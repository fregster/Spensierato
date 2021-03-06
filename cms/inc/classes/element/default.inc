<?php
DEFINE('DEFAULT_TEXT', 'Enter your text here');
class element_Default
{
	//useful when not in admin mode
	protected $id;
	protected $data;
	protected $data_array;
	protected $type;
	protected $name;
	protected $main = 0;
	protected $link = null;
	protected $global = false;
	protected $site_wide_supported = false;
	protected $position = 1;
	protected $title = 'Please give the element a meaningful title';
	protected $options = array();

	private $tags = array();
	private $element;

	//useful only in admin mode
	//put all vars which are only used in admin mode in this array to conserve RAM when not in admin mode
	protected $admin_mode_data = array();

	public function __construct()
	{
		$this->type = __CLASS__;
		$this->name = 'Default Element';

		if (options::singleton()->get_option('admin_mode')) {
			$this->admin_mode_data['image'] = 'accept';
			$this->admin_mode_data['image_edit'] = 'page_edit';
			$this->admin_mode_data['edit_height'] = 525;
			$this->admin_mode_data['edit_width'] = 775;

			$this->admin_mode_data['error'] = 'There was an error comitting your save';
		}

		$this->add_header();
	}

	protected function allowed_tags($tags)
	{
		foreach($tags as $key => $tag)
		{
			if(is_numeric($key)) { $key = $tag; $tag = 'auto'; }
			$this->tags[$key] = $tag;
		}
	}
	
	public function allows_attachments()
	{
		if(array_search('file', $this->tags)){
			return true;
		}
		return false;
	}

	public function create($main_page, $redirect = true)
	{
		$time = time();
		$this->id = database::Singleton()->sql_insert('tbl_elements', array('type', 'created', 'creator'), array($this->type, $time, user::Singleton()->id()));
		$sort = database::Singleton()->sql_max_field('tbl_page_elements', 'sort', array('page_id'), array($main_page));

		database::Singleton()->sql_insert('tbl_page_elements', array('page_id', 'element_id', 'main','ts', 'sort'), array($main_page, $this->id, 1, $time, $sort+1));
		database::Singleton()->sql_insert('tbl_elements_data', array('id', 'data'), array($this->id, '[]'));
		if($redirect)
		{
			common::redirect($main_page);
		}
		return true;
	}

	public function build($id, $data = array(), $link = NULL, $position = NULL, $global = NULL)
	{
		$this->id = $id;
		
		if(count($this->options > 0)){
			$this->allowed_tags(array('options' => 'options'));
		}

		if($position != NULL)
		{
			$this->position = $position;
		}
		
		$this->init_admin_data();

		if($link != NULL)
		{
			$this->link = $link;
		}
		else
		{
			$this->link = Database::Singleton()->sql_select_first(array('*'), 'tbl_page_elements', array('element_id', 'main'), array($this->id, 1));
			if($this->link['page_id'] == currentPage::Singleton()->id()) { $this->main = 1; }
		}
		
		if($global != NULL)
		{
			$this->global = $global;
		}
		else
		{
			$this->global = Database::Singleton()->sql_select_first(array('page_id'), 'tbl_page_elements', array('element_id', 'page_id'), array($this->id, 0));
		}

		if (empty($data) && !empty($_POST)) {
			$data = $_POST;
		}

		$this->data = $data;
		$this->get_data();
		$this->build_options();
		$this->build_data_xml();
		$this->add_late_header();
		
		Content::Singleton()->attach_content($this->render());
	}

	public function main()
	{
		return $this->link['page_id'];
	}

	public function render()
	{
		//Create DOM
		$dom = new DOMDocument();
		$dom->formatOutput = false;

		//Create element and set required attributes
		$this->element = $dom->createElement('element');
		$this->element->setAttribute('id', $this->id);
		$this->element->setAttribute('type', $this->type);
		$this->element->setAttribute('etype', $this->name);
		$this->element->setAttribute('title', $this->title);
		$this->element->setAttribute('sort', $this->position);
		$this->element->setAttribute('main', $this->main);

		$this->admin($dom);

		//Transform the data field
		$this->transform_data();

		//Create XML tag
		$element_data = $dom->createDocumentFragment();

		//Attach the data
		if(strlen($this->data) > 0)
		{
			@$element_data->appendXML($this->data);
			@$this->element->appendChild($element_data);
		}
		//Attach the element to the root dom
		$dom->appendChild($this->element);
		
		//Return the completed XML of the document
		return $dom->saveXML();
	}

	protected function attach_upload()
	{
		//Does this element type possibly have files
		if(isset($this->tags['file']))
		{
			//PHP reports no error uploading the file
			if($_FILES['uploadedfile']['error'] == 0)
			{
				$file_size = (int)$_FILES['uploadedfile']['size'];
				$max_size = (int)Settings::Singleton()->get_setting('max_file_size');
				if($file_size <= $max_size)
				{
					$path = $_FILES['uploadedfile']['tmp_name'];
					$target = Settings::singleton()->get_setting('cms_folder_uploads').DS.$this->id.'-'.$this->admin_mode_data['revision'];
					if(filetransfer::singleton()->move_file($path, $target)){
						unset($this->admin_mode_data['error']);
						return true;
					}

					$this->admin_mode_data['error'] = 'File was unable to be moved, upload failed. Either a Virus or Malware was found or the file system permissions are incorrect';
					return false;
				}
			}
			else
			{
				//Set the error message
				switch ($_FILES['uploadedfile']['error'])
				{
					case 1:
						$this->admin_mode_data['error'] = 'The uploaded file exceeds the upload_max_filesize directive in php.ini. ';
						break;

					case 2:
						$this->admin_mode_data['error'] = 'The uploaded file exceeds the MAX_FILE_SIZE directive that was specified in the HTML form. ';
						break;

					case 3:
						$this->admin_mode_data['error'] = 'The uploaded file was only partially uploaded. ';
						break;

					case 4:
						//This is not an error so return true and escape
						$this->admin_mode_data['error'] = 'No file was uploaded';
						return true;
						break;

					case 6:
						$this->admin_mode_data['error'] = 'Missing a temporary upload folder.';
						break;

					case 7:
						$this->admin_mode_data['error'] = 'Failed to write file to disk.';
						break;

					case 8:
						$this->admin_mode_data['error'] = 'File upload stopped by extension.';
						break;

					default:
						$this->admin_mode_data['error'] = 'Unknow upload error';
						break;
				}
			}

			$this->admin_mode_data['error'] .= '(Unknown error code: '.$_FILES['uploadedfile']['error'].')';
			//Upload failed return false
			return false;
		}

		//no uploads return true to allow outer function to continue.
		return true;
	}

	private function save_upload()
	{
		if(isset($this->tags['file']))
		{
			return filetransfer::singleton()->save_file($this->id, $this->admin_mode_data['revision']);
		}
		return TRUE;
	}

	public function toggle($what)
	{
		switch ($what)
		{
			case 'visibility':
				return $this->toggle_visibility();

			case 'delete':
				return $this->toggle_deleted();

			case 'quickapprove':
				return revisions::approve_revision($this->admin_mode_data['approve']);
				
			case 'sitewide':
				return $this->toggle_sitewide();

			default:
				return false;
		}
	}
	
	protected function reindex_sort()
	{
		
	}
	
	protected function toggle_sitewide()
	{
		if(security::singleton()->can_approve_element($this->id))
		{
			$result = Database::singleton()->sql_select('id', 'tbl_page_elements', array('element_id', 'page_id'), array($this->id, 0));
			
			if(!isset($result[0])){
				$sort = database::Singleton()->sql_max_field('tbl_page_elements', 'sort', array('page_id'), array(0));
				Database::singleton()->sql_insert('tbl_page_elements', array('element_id', 'page_id', 'main', 'ts', 'sort'), array($this->id, 0, 0, CMS::$time, ($sort+1)));
				//Make
				return true;
			}
			
			Database::singleton()->sql_delete_where('tbl_page_elements', array('element_id', 'page_id'), array($this->id, 0));
			//Delete
			return true;
		}
		
		return false;
	}

	protected function toggle_visibility()
	{
		$new_visibility = ($this->admin_mode_data['visible'] - 1)*(-1);
		return database::Singleton()->sql_update_by_id($this->id, 'tbl_elements', array('visible'), array($new_visibility));
	}

	protected function toggle_deleted()
	{
		if($this->admin_mode_data['deleted'] == (NULL or 0))
		{
			$this->admin_mode_data['deleted'] = CMS::Singleton()->time();
		}
		else
		{
			$this->admin_mode_data['deleted'] = 0;
		}
		return database::Singleton()->sql_update_by_id($this->id, 'tbl_elements', array('deleted'), array($this->admin_mode_data['deleted']));
	}

	//add extra data when in admin mode
	public function admin($dom)
	{
		if(options::Singleton()->get_option('admin_mode'))
		{
			$this->element->setAttribute('created', Common::timestamp_to_date($this->admin_mode_data['created']));
			$this->element->setAttribute('creator', $this->admin_mode_data['creator']);
			$this->element->setAttribute('sw', $this->site_wide_supported);

			if($this->admin_mode_data['edited'] != NULL)
			{
				$this->element->setAttribute('edited', Common::timestamp_to_date($this->admin_mode_data['edited']));
				$this->element->setAttribute('editor', $this->admin_mode_data['editor']);
			}
			
			//Attach any options
			//Create XML tag
			$element_options = $dom->createDocumentFragment();
	
			//Attach the options array
			if(count($this->options > 0))	
			{
				$options_array = array();
				foreach($this->options as $key => $option){
					$options_array[] = array('name' => $key, 'values' => $option);
				}
				
				@$element_options->appendXML(xml::array_to_xml_simple($options_array, 'options', 'option'));
				@$this->element->appendChild($element_options);
			}
			//Attach the element to the root dom
			$dom->appendChild($this->element);

			$build_rollback = $this->get_revision();

			//only get revisions list and approvals if the user is an approver
			if (security::singleton()->can_approve()) {
				//attach the archive data if exists
				if ($build_rollback) {
					$rev_history = revisions::get_revisions_list('archive', array('id', 'title', 'revisor', 'revised'), array('approved'), array('1'));
					if (!empty($rev_history)) {
						$archive_data = $dom->createDocumentFragment();
						$archive_data->appendXML($rev_history);
						$this->element->appendChild($archive_data);
					}
				}

				//attach the pending approvals data if exist
				if($this->check_pending_approval()){
					$approvals_list = revisions::get_revisions_list('approval', array('id', 'revisor', 'revised'), array('approved'), array('0'));
					if (!empty($approvals_list)) {
						$approvals = $dom->createDocumentFragment();
						$approvals->appendXML($approvals_list);
						$this->element->appendChild($approvals);
					}
				}
				$this->element->setAttribute('new_v_available', $this->admin_mode_data['approve']);
			}

			if(isset($this->admin_mode_data['approved']))
			{
				$this->element->setAttribute('approved', Common::timestamp_to_date($this->admin_mode_data['approved']));
			}

			if(isset($this->admin_mode_data['approver']))
			{
				$this->element->setAttribute('approver', $this->admin_mode_data['approver']);
			}

			$this->element->setAttribute('visible', $this->admin_mode_data['visible']);
			$this->element->setAttribute('deleted', $this->admin_mode_data['deleted']);
			$this->element->setAttribute('eh', $this->admin_mode_data['edit_height']);
			$this->element->setAttribute('ew', $this->admin_mode_data['edit_width']);
			$this->element->setAttribute('image', $this->admin_mode_data['image']);
			$this->element->setAttribute('image_edit', $this->admin_mode_data['image_edit']);
			$this->element->setAttribute('draft_available', $this->admin_mode_data['draft']);
			$this->element->setAttribute('revision', $this->admin_mode_data['revision']);
		}
	}

	public function reload_element_data($columns = array('data'))
	{
		database::Singleton()->sql_delete_where('tbl_elements_draft', array('element_id', 'user_id'), array($this->id, user::Singleton()->id()));

		$data = database::singleton()->sql_select_first($columns, 'tbl_elements_data', array('id'), $this->id);

		return $data;
	}

	//Overwrite this function in each element to transform the data XML to what ever needs to be sent to the XSLT processor
	protected function transform_data()
	{

	}

	//seperated from arrange_data()
	protected function check_pending_approval(){
		$pending_id = database::Singleton()->sql_select_first(array('id'), 'tbl_elements_revisions', array('element_id', 'approved'), array($this->id, '0'), array('id'), array('DESC'));

		if ($pending_id != NULL) {
			$this->admin_mode_data['approve'] = $pending_id['id'];
		}

		return $this->admin_mode_data['approve'];
	}

	protected function get_revision()
	{
		$rev_id = Database::Singleton()->sql_select(array('active_revision'), 'tbl_elements', array('id'), array($this->id), array('id'), array('DESC'));
		if ($rev_id != NULL) {
			//get the highest approved revision id
			//this is at index 0 because of "ORDER BY `id` DESC" in the qiery
			$this->admin_mode_data['revision'] = $rev_id[0]['active_revision'];
		}

		//this return controls whether we display rollback interface or not.
		//we only want to do this if there are > 1 revisions
		if (count($rev_id) > 1) {
			return TRUE;
		}
		else{
			return FALSE;
		}
	}

	protected function add_header()
	{

	}
	
	protected function add_late_header()
	{
		
	}

	protected function attach_extra_data()
	{
		return array();
	}

	protected function get_data(){
		//if there is no preset data, get data from the DB
		if (empty($this->data)) {
			$data = NULL;

			if (options::Singleton()->get_option('admin_mode')) {

				//check if there is a saved draft for the user
				$data = database::Singleton()->sql_select_first(array('title', 'data', 'ts'), 'tbl_elements_draft', array('element_id', 'user_id'), array($this->id, user::Singleton()->id()));

				//if there is a draft, set the draft timestamp
				if ($data != NULL) {
					$this->admin_mode_data['draft'] = $data['ts'];
				}
				//if there is no draft, and the user is not an approver,
				//check if there is approval data for this user
				else if (security::singleton()->can_edit() && !security::singleton()->can_approve()) {
					$data = database::Singleton()->sql_select_first(array('title', 'data'), 'tbl_elements_revisions', array('element_id', 'revisor', 'approved'), array($this->id, user::singleton()->id(), '0'), array('id'), array('DESC'));
				}
				
			}

			//if no data was set yet, just get the default element data
			if ($data == NULL || !$data) {
				$data = database::Singleton()->sql_select_first(array('title', 'data'), 'tbl_elements_data', array('id'), array($this->id));
			}

			$this->data = json_decode($data['data'], TRUE);

			if ($data['title'] != NULL) {
				$this->title = $data['title'];
			}

		}
		else if (is_string($this->data)) {
			$this->data = json_decode($this->data, TRUE);
		}
		else if (isset($this->data['data'])) {
			$html = json_decode($this->data['data'], TRUE);
			if($html != NULL && is_array($html))	{
				$this->data = array_merge($this->data, $html);
			}
		}
		else if (isset($this->data['raw'])) {
			$copy = $this->data['raw'];
			$this->data = $copy;
			unset($copy);
		}
		
		//make sure the data conforms to the element data schema
		if (!empty($this->tags)) {
			foreach ($this->tags as $key => $value){
				if (!isset($this->data[$key])) {
					$this->data[$key] = '';
				}
			}
		}

		//Only process the data if there is actually any data else blank pages error
		if(is_array($this->data)){

			//save the title
			if (array_key_exists('title', $this->data)) {
				if (!empty($this->data['title'])) {
					$this->title = $this->data['title'];
				}
				unset($this->data['title']);
			}

			//delete any data that is not in the element data schema
			foreach ($this->data as $key => $value){
				if (!array_key_exists($key, $this->tags)) {
					unset($this->data[$key]);
				}
			}

			if (options::singleton()->get_option('admin_mode')){
				//if admin mode is on, save array for saving process before converting to xml
				$this->admin_mode_data['save_data'] = $this->data;

			}
		}
	}
	
	protected function build_options(){
		foreach($this->options as $key => $option){
			if(!isset($this->data['options'][$key]) || !in_array($this->data['options'][$key], $option)){
				if(is_null($option[0])){
					unset($this->data['options'][$key]);
				}else{
					$this->data['options'][$key] = $option[0];
				}
			}
		}
	}

	protected function build_data_xml(){
		if(is_array($this->data)){
			$this->data_array = $this->data; //make a copy of the data in to an array
			$this->data = stripslashes(xml::array_to_xml_simple($this->data, 'data'));  //FIXME Should not need strip slashes here the database should return the data as we saved it
	
			//FIXME: try to come up with a better way to add attributes to xml from arrays
			foreach ($this->tags as $tag => $type){
				$this->data = str_replace("<$tag>", "<$tag type=\"$type\">", $this->data);
			}
		}
	}
	
	private function save_options(){
		$save_option = array();
		
		foreach($this->options as $key => $posible_options){
			$post_name = 'option_'.$key;
			if(post::variable($post_name, 'isset') && in_array(post::variable($post_name), $this->options[$key])){
				$save_option[$key] = post::variable($post_name);
			}
		}
		
		return $save_option;
	}

	public function save($draft = TRUE){

		if(!$this->attach_upload())
		{
			//FIXME: update to use debug / error class
			$error = 'Error uploading';
			if(isset($this->admin_mode_data['error']) && strlen($this->admin_mode_data['error']) > 1){
				$error .= ' ('.$this->admin_mode_data['error'].')';
			}
			die($error);
		}

		$details = array_merge($this->admin_mode_data['save_data'], $this->attach_extra_data());
//		$search = $this->build_search_text($details);
		//process the data being saved
		foreach($details as $key => &$value)
		{
			if($value == DEFAULT_TEXT) { $value = null; }
			switch ($key)
			{					
				case 'html':
					if( (Settings::Singleton()->get_setting('tidy_html_input')) and (function_exists('tidy_parse_string')) && !post::variable('skip_validation_html', 'isset'))
					{
						/**
						 * Set configuration options for HTML tidy.
						 */
						$config = array('indent' => false,
										'doctype' => 'omit',
										'output-xhtml' => true,
										'wrap' => 0,
										'bare' => true,
										'drop-font-tags' => true,
										'drop-empty-paras' => true,
										'hide-comments'=> true,
										'join-classes' => true,
										'join-styles' => true,
										'word-2000' => true,
										'fix-backslash' => true,
										'show-body-only' => true,
										'doctype' => 'strict');
			
						/**
						 * Run HTML tidy on the html code
						 */
						
						$tidy = tidy_parse_string($value, $config, 'UTF8');
						$tidy->cleanRepair();
			
						$value = $tidy;
						unset($tidy);
					}
					
					$value = Common::html($value);
					break;

				case 'href':
					//matches http://, https://, ftp://.
					//to add more protocols, add them to the |-seperated list in the ()
					if(preg_match('#^(https?|ftp)://#', $value)){  }
					else
					{	//delete any protocol in the url and prefix with http://
						$value = preg_replace('#^[a-z]*://#', '', $value);
						$value = 'http://'.$value;
					}
					break;

				case 'email':
					if (!common::email_address_check($value)) {
						//do something about invalid emails
						$value = NULL;
					}
					break;
					
				case 'options':
					$value = $this->save_options();
					break;
			}
		}

//		$details['search'] &= $search;
		$data = json_encode($details);
		unset($details);
//		unset($search);

		if($draft)
		{
			database::Singleton()->sql_update_or_insert('tbl_elements_draft', array('element_id', 'user_id'), array($this->id, User::Singleton()->id()), array('user_id', 'element_id', 'data', 'ts', 'title'), array(User::Singleton()->id(), $this->id, $data, CMS::Singleton()->time(), $this->title));
			die('SAVED');
		}
		else
		{
			database::Singleton()->sql_delete_where('tbl_elements_draft', array('element_id', 'user_id'), array($this->id, user::Singleton()->id()));

			if ($this->save_upload()) 
			{
				$revision_id = database::Singleton()->sql_update_or_insert('tbl_elements_revisions', array('element_id', 'revisor', 'approved'), array($this->id, User::Singleton()->id(), '0'), array('element_id', 'data', 'title', 'revisor', 'revised'), array($this->id, $data, $this->title, User::Singleton()->id(), CMS::Singleton()->time()), false, 'id');

				//save if has permissions
				if (User::singleton()->is_admin() || (settings::singleton()->get_setting('content_auto_publish') && security::Singleton()->can_do('approve'))) {
					revisions::approve_revision($revision_id);
				}

				return TRUE;
			}
			return FALSE;
		}
	}

	protected function init_admin_data(){
		if (options::singleton()->get_option('admin_mode')) {
			$element_details = database::Singleton()->sql_select_first(array('*'), 'tbl_elements', array('id'), array($this->id));

			//save admin_mode data
			$this->admin_mode_data['created'] = $element_details['created'];
			$this->admin_mode_data['creator'] = user::Singleton()->username($element_details['creator']);
			$this->admin_mode_data['edited'] = $element_details['edited'];
			$this->admin_mode_data['editor'] = user::Singleton()->username($element_details['editor']);
			$this->admin_mode_data['approved'] = $element_details['approved'];
			$this->admin_mode_data['approver'] = user::Singleton()->username($element_details['approver']);
			$this->admin_mode_data['visible'] = $element_details['visible'];
			$this->admin_mode_data['deleted'] = $element_details['deleted'];
			unset($element_details);

			//initialize admin_mode data not present in the DB
			$this->admin_mode_data['revision'] = NULL;
			$this->admin_mode_data['draft'] = NULL;
			$this->admin_mode_data['approve'] = NULL;
		}
	}
	
	/**
	 * Generate the searchable text, this is done here as each element type might want to extend this
	 * @param $details
	 * @return unknown_type
	 */
	protected function build_search_text($details){
//		$details = xml::xml_to_array($details);
		$html = xml::node_from_xml($details, 'html');

		if(isset($this->tags['html']) and isset($html[0]))
		{
			return common::strip_non_alpha_numeric(strip_tags($html[0]));
		}
		return false;
	}
	
	protected function build_search_summary(){
		if($this->title != 'Please give the element a meaningful title')
		return $this->title;
	}
	
	public function get_search_text($expand_files = false)
	{
		if(($expand_files == false) && ($this->allows_attachments())) //If this is a file and we are not expanding it then only return the summary data
		{
			return $this->build_search_summary();
		}
		return $this->build_search_text($this->data);
	}
	
	public function title()
	{
		return $this->title;
	}
}