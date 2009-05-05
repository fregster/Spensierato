<?php
class elementDefault
{
	//useful when not in admin mode
	protected $id;
	protected $data;
	protected $type;
	protected $name;
	protected $title = 'Please give the element a meaningful title';

	private $tags = array('title' => 'text');
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
			$this->admin_mode_data['edit_height'] = 400;
			$this->admin_mode_data['edit_width'] = 725;

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

	public function build($id, $data = array(), $link = NULL)
	{
		$this->id = $id;

		if($link == NULL)
		{
			$this->link = Database::Singleton()->sql_select_first(array('*'), 'tbl_page_elements', array('element_id', 'main'), array($id, 1));
		}
		else
		{
			$this->link = $link;
		}

		if (empty($data) && !empty($_POST)) {
			$data = $_POST;
		}

		$this->data = $data;
		$this->get_data();

		$this->init_admin_data();

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

		$this->admin(&$dom);

		//Transform the data field
		$this->transform_data();

		//Create XML tag
		$element_data = $dom->createDocumentFragment();

		//Attach the data
		$element_data->appendXML($this->data);
		$this->element->appendChild($element_data);

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
					$target = Settings::singleton()->get_setting('cms_folder_uploads').$this->id.'-'.$this->admin_mode_data['revision'];
					return(filetransfer::move_file($path, $target));
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
				return $this->approve();

			default:
				return false;
		}
	}

	protected function toggle_visibility()
	{
		$new_visibility = ($this->visible - 1)*(-1);
		return database::Singleton()->sql_update_by_id($this->id, 'tbl_elements', array('visible'), array($new_visibility));
	}

	protected function toggle_deleted()
	{
		if($this->deleted == (NULL or 0))
		{
			$this->deleted = CMS::Singleton()->time();
		}
		else
		{
			$this->deleted = 0;
		}
		return database::Singleton()->sql_update_by_id($this->id, 'tbl_elements', array('deleted'), array($this->deleted));
	}

	protected function generate_search($input)
	{	//exchange </tag><tag> couples with spaces
		$input = preg_replace('#</\w+><\w+>#', ' ', html_entity_decode($input));
		return strip_tags($input);
	}

	protected function archive_data($new_data){
		$old_data = $this->reload_element_data(array('data', 'title'));
		if ($old_data != NULL && ($old_data['data'] != '[]') && ($old_data['data'] != $new_data)) {
			database::Singleton()->sql_insert('tbl_elements_data_archive',
									array('data', 'title', 'ts', 'replaced_by', 'replaced_ts', 'element_id'),
									array($old_data['data'], $old_data['title'], $this->admin_mode_data['approved'], user::Singleton()->id(), time(), $this->id));
		}
	}

	//add extra data when in admin mode
	public function admin($dom)
	{
		if(options::Singleton()->get_option('admin_mode'))
		{
			$this->element->setAttribute('created', $this->admin_mode_data['created']);
			$this->element->setAttribute('creator', $this->admin_mode_data['creator']);

			if($this->admin_mode_data['edited'] != NULL)
			{
				$this->element->setAttribute('edited', $this->admin_mode_data['edited']);
				$this->element->setAttribute('editor', $this->admin_mode_data['editor']);
			}

			$this->get_revision();

			//only get revisions list and approvals if the user is an approver
			if (security::singleton()->can_approve()) {
				//attach the archive data if exists
				if ($this->admin_mode_data['revision']) {
					$rev_history = $this->get_revision_history();
					if (!empty($rev_history)) {
						$archive_data = $dom->createDocumentFragment();
						$archive_data->appendXML($rev_history);
						$this->element->appendChild($archive_data);
					}
				}

				//attach the pending approvals data if exist
				if($this->check_pending_approval()){
					$approvals_list = $this->get_pending_approval_list();
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
				$this->element->setAttribute('approved', date(settings::singleton()->get_setting('date_format'), $this->admin_mode_data['approved']));
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
		$approvals = database::Singleton()->sql_select(array('id', 'ts', 'user_id', 'approved'), 'tbl_elements_approve', array('element_id'), array($this->id), array('ts'), array('DESC'));

		if ($approvals != NULL) {

			if ($approvals[0]['approved']) {
				$this->admin_mode_data['approve'] = FALSE;
			}
			else{
				$this->admin_mode_data['approve'] = TRUE;
			}

			//only save un-approved edits as available approvals
			foreach ($approvals as $key => $approval){
				if ($approval['approved']) {
					unset($approvals[$key]);
				}
			}
			$this->admin_mode_data['approvals_list'] = $approvals;

			return $this->admin_mode_data['approve'];
		}

		return NULL;
	}

	protected function get_pending_approval_list(){
		$approval_list = $this->admin_mode_data['approvals_list'];
		if ($approval_list && options::singleton()->get_option('admin_mode')) {

			$latest_edit = ($this->approved < $this->edited)?$this->edited:$this->approved;
			$show_old = settings::singleton()->get_setting('approvals_show_old');

			$built_xml = '<pending_approvals show_old="'.$show_old.'">';
			foreach ($approval_list as $key => $pending_approval){
				$built_xml .= '<approval>';
				$built_xml .= '<id>'.$pending_approval['id'].'</id>';
				if ($pending_approval['ts'] < $latest_edit) {
					$built_xml .= '<old />';
				}
				$built_xml .= '<date>'.date(settings::singleton()->get_setting('date_format'), $pending_approval['ts']).'</date>';
				if (settings::singleton()->get_setting('show_username_in_approvals')) {
					$user = database::Singleton()->sql_select_first(array('username'), 'tbl_users', array('id'), array($pending_approval['user_id']));
					$built_xml .= '<username>'.$user['username'].'</username>';
				}
				if ($key === 0) {
					$built_xml .= '<checked />';
				}
				$built_xml .= '</approval>';
			}
			$built_xml .= '</pending_approvals>';
			return $built_xml;
		}
	}

	protected function get_revision()
	{
		$this->admin_mode_data['revision'] = Database::Singleton()->sql_count_where('tbl_elements_data_archive', 'element_id', $this->id, options::Singleton()->get_option('admin_mode'));
		return is_null($this->admin_mode_data['revision'])?FALSE:TRUE;
	}

	protected function get_revision_history()
	{
		if(options::Singleton()->get_option('admin_mode'))
		{
			$built_xml = '';
			$revisions = database::Singleton()->sql_select(array('title', 'replaced_ts', 'replaced_by'), 'tbl_elements_data_archive', array('element_id'), array($this->id));
			if ($revisions) {
				$built_xml .= '<archive>';
				foreach ($revisions as $revision_num => $data){
					$user = database::Singleton()->sql_select_first(array('username'), 'tbl_users', array('id'), array($data['replaced_by']));
					$built_xml .= '<revision>';
					$built_xml .= '<id>'.($revision_num + 1).'</id>';

					if (strlen($data['title']) > 5) {
						$data['title'] = substr($data['title'], 0, 4).'...';
					}

					$built_xml .= '<title>'.$data['title'].'</title>';
					$built_xml .= '<user>'.$user['username'].'</user>';
					$built_xml .= '<date>'.date(settings::singleton()->get_setting('date_format'), $data['replaced_ts']).'</date>';
					$built_xml .= '</revision>';
				}
				$built_xml .= '</archive>';
			}
			return $built_xml;
		}
	}

	protected function add_header()
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
					$data = database::Singleton()->sql_select_first(array('title', 'data'), 'tbl_elements_approve', array('element_id', 'user_id', 'approved'), array($this->id, user::singleton()->id(), '0'), array('id'), array('DESC'));
				}
			}

			//if no data was set yet, just get the default element data
			if ($data == NULL) {
				$data = database::Singleton()->sql_select_first(array('title', 'data'), 'tbl_elements_data', array('id'), array($this->id));
			}

			$this->data = json_decode($data['data'], TRUE);
			if(isset($this->data['title']))
			{
				$this->title = $this->data['title'];
				unset($this->data['title']);
			}
		}
		else if (is_string($this->data)) {
			$this->data = json_decode($this->data, TRUE);
		}
		else if (isset($this->data['data'])) {
			$html = json_decode($this->data['data'], TRUE);
			if(is_array($html))  //Tried isset($html[0]) but still raised a notice
			{
				$this->data = array_merge($this->data, $html);
			}
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

			//delete any data that is not in the element data schema
			foreach ($this->data as $key => $value){
				if (!array_key_exists($key, $this->tags)) {
					unset($this->data[$key]);
				}
			}

			//save the title
			if (array_key_exists('title', $this->data)) {
				$this->title = $this->data['title'];
				unset($this->data['title']); //uncomment this when we finish moving title from node to attribute
			}

			//if admin mode is on, save array for saving process before converting to xml
			if (options::singleton()->get_option('admin_mode')){
				$this->admin_mode_data['save_data'] = $this->data;
			}

			$this->build_data_xml();
		}
	}

	protected function build_data_xml(){
		$this->data = common::array_to_xml_simple($this->data, 'data');

		//FIXME: try to come up with a better way to add attributes to xml from arrays
		foreach ($this->tags as $tag => $type){
			$this->data = str_replace("<$tag>", "<$tag type=\"$type\">", $this->data);
		}
	}

	public function save($draft = TRUE){

		if(!$this->attach_upload())
		{
			die($this->admin_mode_data['error']);
		}

		$details = array_merge($this->admin_mode_data['save_data'], $this->attach_extra_data());

		//delete this conditional when we finish moving title from element to attribute
		if (isset($details['title'])) {
			unset($details['title']);
		}

		//process the data being saved
		foreach($details as $key => &$value)
		{
			switch ($key)
			{
				case 'html':
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
			}
		}

		$data = json_encode($details);

		if($draft)
		{
			database::Singleton()->sql_update_or_insert('tbl_elements_draft', array('element_id', 'user_id'), array($this->id, User::Singleton()->id()), array('user_id', 'element_id', 'data', 'ts', 'title'), array(User::Singleton()->id(), $this->id, $data, CMS::Singleton()->time(), $this->title));
			die('SAVED');
		}
		else
		{
			database::Singleton()->sql_delete_where('tbl_elements_draft', array('element_id', 'user_id'), array($this->id, user::Singleton()->id()));

			if ($this->save_upload()) {

				database::Singleton()->sql_update_or_insert('tbl_elements_approve', array('element_id', 'user_id'), array($this->id, User::Singleton()->id()), array('user_id', 'element_id', 'data', 'ts', 'title'), array(User::Singleton()->id(), $this->id, $data, CMS::Singleton()->time(), $this->title));

				//save if has permissions
				if (User::singleton()->is_admin() || (settings::singleton()->get_setting('content_auto_publish') && security::Singleton()->can_do('approve'))) {
					$this->approve();
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
			$this->admin_mode_data['creator'] = $element_details['creator'];
			$this->admin_mode_data['edited'] = $element_details['edited'];
			$this->admin_mode_data['editor'] = $element_details['editor'];
			$this->admin_mode_data['approved'] = $element_details['approved'];
			$this->admin_mode_data['approver'] = $element_details['approver'];
			$this->admin_mode_data['visible'] = $element_details['visible'];
			$this->admin_mode_data['deleted'] = $element_details['deleted'];
			unset($element_details);

			//initialize admin_mode data not present in the DB
			$this->admin_mode_data['revision'] = NULL;
			$this->admin_mode_data['draft'] = NULL;
			$this->admin_mode_data['approve'] = NULL;
		}
	}

	//TODO: make this work for multiple approvals
	public function approve($approval_id = NULL){

		$where_data = array('fields' => array('element_id'), 'values' => array($this->id));

		if (is_numeric($approval_id)) {
			$where_data['fields'][] = 'id';
			$where_data['values'][] = $approvals_id;
		}

		$approved_data = database::Singleton()->sql_select_first(array('id', 'user_id', 'ts', 'data', 'title'), 'tbl_elements_approve', array('element_id'), array($this->id), array('id'), array('DESC'));

		$this->archive_data($approved_data['data']);

		database::Singleton()->sql_update_by_id($this->id, 'tbl_elements', array('edited', 'editor', 'approved', 'approver'), array($approved_data['ts'], $approved_data['user_id'], time(), User::Singleton()->id()));
		database::Singleton()->sql_update_by_id($this->id, 'tbl_elements_data', array('data', 'title','search'), array($approved_data['data'], $approved_data['title'], $this->generate_search($approved_data['data'])));

		database::Singleton()->sql_update_by_id($approved_data['id'], 'tbl_elements_approve', array('approved'), array('1'));

		return TRUE;
	}
}
?>