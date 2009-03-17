<?php
class elementDefault
{
	protected $id;
	protected $data;
	protected $details;
	protected $type;
	protected $name;
	protected $image;
	protected $image_edit;
	protected $edit_height;
	protected $edit_width;
	protected $visible;
	protected $deleted;
	protected $created;
	protected $creator;
	protected $edited;
	protected $editor;
	protected $approved;
	protected $approver;
	protected $revision;
	protected $draft;
	protected $approve;
	protected $revision_history;

	protected $error = 'There was an error comitting your save';

	private $tags = array('title' => 'text');
	private $element;

	function __construct()
	{
		$this->type = __CLASS__;
		$this->name = 'Default Element';
		$this->image = 'accept';
		$this->image_edit = 'page_edit';
		$this->edit_height = 400;
		$this->edit_width = 700;

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
		database::Singleton()->sql_insert('tbl_elements_data', array('id', 'data'), array($this->id, '<null />'));
		if($redirect)
		{
			common::redirect($main_page);
		}
		return true;
	}

	public function build($id, $data = NULL, $link = NULL)
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

		$this->get_data($data);
		$this->check_pending_approval();
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
		$this->element->setAttribute('eh', $this->edit_height);
		$this->element->setAttribute('ew', $this->edit_width);
		$this->element->setAttribute('image', $this->image);
		$this->element->setAttribute('title', $this->title);

		$this->admin();

		//Transform the data field
		$this->transform_data();

		//Create XML tag
		$element_data = $dom->createDocumentFragment();

		//Attach the data
		$element_data->appendXML($this->data);
		$this->element->appendChild($element_data);

		//attach the archive data if exists
		$this->get_revision_history();
		if ($this->revision_history) {
			$archive_data = $dom->createDocumentFragment();
			$archive_data->appendXML($this->revision_history);
			$this->element->appendChild($archive_data);
		}

		//Attach the element to the root dom
		$dom->appendChild($this->element);

		//Return the completed XML of the document
		return $dom->saveXML();
	}

	public function save($draft = true)
	{
		if($this->attach_upload() == false)
		{
			die($this->error);
			return false;
		}
		$extra = $this->attach_extra_data();

		$details = array_merge($_POST, $extra);

		//get the title and remove it from the array so it
		//doesn't get into the xml, since it isn't part of the data
		if ($details['title'] == 'Please give the element a meaningful title') {
			$details['title'] = NULL;
		}
		$title = $details['title'];
		unset($details['title']);

		//get the data that will be saved in the 'data' field in the db and construct it as xml
		$xml = '';
		foreach($details as $key => $value)
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
			$xml .= "<$key>".utf8_encode(common::sql_clean(strip_tags($value)))."</$key>";
		}

		if($draft)
		{
			//database::Singleton()->sql_update_or_insert('cms_tbl_elements_draft', array('element_id', 'user_id'), array($this->id,user::Singleton()->id()), array('data', 'ts'), array(common::sql_clean(strip_tags(htmlentities($data, ENT_QUOTES))), CMS::Singleton()->time()));
			//database::Singleton()->sql_update_by_id($this->id, $table, $fields = array(), $values = array())
			database::Singleton()->sql_update_or_insert('tbl_elements_draft', array('element_id', 'user_id'), array($this->id, User::Singleton()->id()), array('user_id', 'element_id', 'data', 'ts', 'title'), array(User::Singleton()->id(), $this->id, $xml, CMS::Singleton()->time(), $title));
//database::Singleton()->sql_update_or_insert('tbl_elements_draft', array('element_id', 'user_id'), array($this->id, User::Singleton()->id()), array('user_id', 'element_id', 'data', 'ts', 'title'), array(User::Singleton()->id(), $this->id, $xml, CMS::Singleton()->time(), $title));
			die('SAVED');
		}
		else
		{
			database::Singleton()->sql_delete_where('tbl_elements_draft', array('element_id', 'user_id'), array($this->id, user::Singleton()->id()));
			if($this->process_save($xml, $title));
			{
				return true;
			}

			return false;
		}

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
					$target = Settings::singleton()->get_setting('cms_folder_uploads').$this->id.'-'.$this->revision;
					return(filetransfer::move_file($path, $target));
				}
			}
			else
			{
				//Set the error message
				switch ($_FILES['uploadedfile']['error'])
				{
					case 1:
						$this->error = 'The uploaded file exceeds the upload_max_filesize directive in php.ini. ';
					break;

					case 2:
						$this->error = 'The uploaded file exceeds the MAX_FILE_SIZE directive that was specified in the HTML form. ';
					break;

					case 3:
						$this->error = 'The uploaded file was only partially uploaded. ';
					break;

					case 4:
						//This is not an error so return true and escape
						$this->error = 'No file was uploaded';
						return true;
					break;

					case 6:
						$this->error = 'Missing a temporary upload folder.';
					break;

					case 7:
						$this->error = 'Failed to write file to disk.';
					break;

					case 8:
						$this->error = 'File upload stopped by extension.';
					break;

					default:
						$this->error = 'Unknow upload error';
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
			return filetransfer::singleton()->save_file($this->id, $this->revision);
		}
	}

	public function toggle($what)
	{
		switch ($what)
		{
			case 'visibility':
				return( $this->toggle_visibility());
				break;

			case 'delete':
				return( $this->toggle_deleted());
				break;
			case 'quickapprove':
				return( $this->toggle_quickapprove());
				break;
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

	protected function toggle_quickapprove(){
		//TO FIX: currently this only approvs the most recent pending edit.
		$approvalData = end(database::Singleton()->sql_select(array('user_id', 'ts', 'data', 'title'), 'tbl_elements_approve', array('element_id'), array($this->id)));
		$this->archive_data($approvealData['data']);
		database::Singleton()->sql_update_by_id($this->id, 'tbl_elements', array('edited', 'editor', 'approved', 'approver'), array($approvalData['ts'], $approvalData['user_id'], time(), User::Singleton()->id()));
		database::Singleton()->sql_update_by_id($this->id, 'tbl_elements_data', array('data', 'title','search'), array($approvalData['data'], $approvalData['title'], $this->generate_search($approvalData['data'])));
		return database::Singleton()->sql_delete_where('tbl_elements_approve', array('element_id'), $this->id);
	}

	protected function generate_search($input)
	{	//exchange </tag><tag> couples with spaces
		$input = preg_replace('#</\w+><\w+>#', ' ', html_entity_decode($input));
		return strip_tags($input);
	}

	protected function process_save($xml, $title)
	{
		$this->save_upload();

		//save if has permissions
		if (security::Singleton()->can_do('approve')) {
			$this->archive_data($xml);
			database::Singleton()->sql_update_by_id($this->id, 'tbl_elements', array('edited', 'editor'), array(time(), User::Singleton()->id()));
			database::Singleton()->sql_update_by_id($this->id, 'tbl_elements_data', array('data', 'title', 'search'), array($xml, $title, $this->generate_search($xml)));
		}
		//else submit for approval
		else {
			database::Singleton()->sql_update_or_insert('tbl_elements_approve', array('element_id', 'user_id'), array($this->id, User::Singleton()->id()), array('user_id', 'element_id', 'data', 'ts', 'title'), array(User::Singleton()->id(), $this->id, $xml, CMS::Singleton()->time(), $title));
		}

		return true;
	}

	protected function archive_data($xml){
		$archive_data = database::Singleton()->sql_select_first(array('id', 'title', 'data'), 'tbl_elements_data', array('id'), array($this->id));
		if ($archive_data && ($archive_data['data'] != '<null />') && ($archive_data['data'] != $xml)) {
			database::Singleton()->sql_insert('tbl_elements_data_archive',
									array('data', 'title', 'ts', 'replaced_by', 'replaced_ts', 'element_id'),
									array($archive_data['data'], $archive_data['title'], time(), user::Singleton()->id(), time(), $archive_data['id']));
		}
	}

	//add extra data when in admin mode
	public function admin()
	{
		if(options::Singleton()->get_option('admin_mode'))
		{
			$this->get_revision();
			$this->element->setAttribute('created', $this->created);
			$this->element->setAttribute('creator', $this->creator);
			if($this->edited != NULL)
			{
				$this->element->setAttribute('edited', $this->edited);
				$this->element->setAttribute('editor', $this->editor);
			}
			if(isset($this->approved))
			{
				$this->element->setAttribute('approved', date(settings::singleton()->get_setting('date_format)'), $this->approved));
			}

			if(isset($this->approver))
			{
				$this->element->setAttribute('approver', $this->approver);
			}

			$this->element->setAttribute('revision', $this->revision);
			$this->element->setAttribute('draft_available', $this->draft);
			$this->element->setAttribute('new_v_available', $this->approve);
			$this->element->setAttribute('image_edit', $this->image_edit);
			$this->element->setAttribute('visible', $this->visible);
			$this->element->setAttribute('deleted', $this->deleted);
		}
	}

	public function ajax_edit()
	{
		database::Singleton()->sql_delete_where('tbl_elements_draft', array('element_id', 'user_id'), array($this->id, user::Singleton()->id()));
		$this->get_data();
		return common::node_from_xml($this->data, '/data/html');
	}

	//Over right this function in each element to transform the data XML to what ever needs to be sent to the XSLT processor
	protected function transform_data()
	{
		//$this->data = $this->data;
	}

	//select the correct data to display
	protected function get_data($data = NULL){

		$general_info = database::Singleton()->sql_select_first(array('visible', 'deleted', 'created', 'creator', 'edited', 'editor', 'approved', 'approver'), 'tbl_elements', array('id'), array($this->id));
		$default_data = database::Singleton()->sql_select_first(array('data', 'title'), 'tbl_elements_data', array('id'), array($this->id));
		$final_data = array();
		$type = NULL;

		//display plain text when not editing
		if (is_null(CMS::Singleton()->path_vars(0)) || is_numeric(CMS::Singleton()->path_vars(0))) {
			if (!is_null($data)) {
				$final_data = array_merge(array('data' => $data, 'title' => $default_data['title']), $general_info);
			}
		}
		else{
			if (options::Singleton()->get_option('admin_mode')) {
				//check if there is a saved draft for the user
				$data = database::Singleton()->sql_select_first(array('data', 'title', 'ts'), 'tbl_elements_draft', array('element_id', 'user_id'), array($this->id, user::Singleton()->id()));
				if (!is_null($data)) {
					$final_data = array_merge($data, $general_info);
					$type = 'draft';
				}
				else{
					//check if there is aproval data
					$whereFields = array('element_id');
					$whereVals = array($this->id);
					//for non-approvers, only show their own changes
					if (!Security::Singleton()->can_do('approve')) {
						array_push($whereFields, 'user_id');
						array_push($whereVals, user::singleton()->id());
					}
					$data = database::Singleton()->sql_select_first(array('data', 'title', 'ts'), 'tbl_elements_approve', $whereFields, $whereVals);
					if (!is_null($data)) {
						if (Security::Singleton()->can_do('approve')) {
							//get text diff for approvers
							$old = strip_tags($default_data['data']);
							$new = strip_tags($data['data']);
							$diff = diffCompare::get_diff($old, $new);
							$data['data'] = diffCompare::format_diff($diff);
						}
						$final_data = array_merge($data, $general_info);
					}
				}
			}
		}

		//if no data was set yet, just get the element data from the db
		if (empty($final_data)) {
			if(!empty($default_data) && !empty($general_info)) { $final_data = array_merge($default_data, $general_info); }
			else
			{ $final_data &= $default_data; }
		}

		$this->arrange_data($final_data, $type);
	}

	//this is what used to be get_data, now only arranges the data as xml
	protected function arrange_data($data, $type = NULL)
	{
		$dbxml = '<data>'.$data['data'].'</data>';

		if(options::Singleton()->get_option('admin_mode'))
		{

			if(isset($data['ts']) && $type == 'draft')
			{
				$this->draft = $data['ts'];
			}

			$built_xml = '<data>';
			foreach($this->tags as $key => $value)
			{
				switch ($key)
				{
					case 'title':
						$this->visible = $data['visible'];
						$this->deleted = $data['deleted'];
						$this->created = $data['created'];
						$this->creator = $data['creator'];
						$this->edited = $data['edited'];
						$this->editor = $data['editor'];
						$this->approved = $data['approved'];
						$this->approver = $data['approver'];
						$data_content = $data['title'];
						break;

					case 'html':
						$data_content = Common::html(html_entity_decode($data['data']));
						break;

					default:
						$data_content = common::node_from_xml($dbxml, '/data/'.$key);
						break;
				}

				$data_content = utf8_decode(strip_tags($data_content));
				$built_xml .= '<'.$key.' type="'.$value.'">'.$data_content.'</'.$key.'>';
			}
			$built_xml .= '</data>';
			$this->data = $built_xml;
		}
		else
		{
			$this->data = $dbxml;
		}

		if(empty($data['title'])) { $this->title = 'Please give the element a meaningful title'; }
		else { $this->title = $data['title']; }

		$this->search = $this->generate_search($data['data']);
	}

	//seperated from arrange_data()
	protected function check_pending_approval(){
		$approvals = database::Singleton()->sql_select_first(array('id'), 'tbl_elements_approve', array('element_id'), array($this->id));
		if (!is_null($approvals) && security::Singleton()->can_do('approve')) {
			$this->approve = true;
		}
	}

	protected function get_revision()
	{
		$this->revision = Database::Singleton()->sql_count_where('tbl_elements_data_archive', 'element_id', $this->id, options::Singleton()->get_option('admin_mode'));
		return $this->revision;
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
			$this->revision_history = $built_xml;
		}
	}

	protected function add_header()
	{

	}

	protected function attach_extra_data()
	{
		return array();
	}

}
?>