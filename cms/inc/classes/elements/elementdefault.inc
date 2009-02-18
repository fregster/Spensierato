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

		$user_draft = database::Singleton()->sql_select_first(array('data'), 'tbl_elements_draft', array('element_id', 'user_id'), array($this->id, user::Singleton()->id()));
		if ($user_draft) {
			$data = $user_draft['data'];
		}

		$this->get_data($data);
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

		//Build posted data as XML structure
		$xml = '<xml>';
		foreach($details as $key => $value)
		{
			switch ($key)
			{
				case 'html':
					$value = Common::html($value);
				break;
				
				case 'href':
					if( (substr($value, 0, 7) == 'http://') or (substr($value, 0, 8) == 'https://') or (substr($value, 0, 6) == 'ftp://') ){  }	else
					{
						$data = 'http://'.$data;
					}
				break;
			}
			$xml .= "<$key>".common::sql_clean(strip_tags($value))."</$key>";
		}
		$xml .= '</xml>';

		//Check data
		foreach($this->tags as $key => $value)
		{
			if($key == 'title')
			{
				$title = new SimpleXMLElement($xml);
				$title = ''.$title->title;
				if(strlen($title) < 2)
				{
					$title = NULL;
				}
			}
			else
			{
				$data = common::node_from_xml($xml, '/xml/'.$key);
				$built_xml .= '<'.$key.'>'.utf8_encode(common::sql_clean(strip_tags(Common::html($data)))).'</'.$key.'>';
			}
		}

		if($draft)
		{
			//database::Singleton()->sql_update_or_insert('cms_tbl_elements_draft', array('element_id', 'user_id'), array($this->id,user::Singleton()->id()), array('data', 'ts'), array(common::sql_clean(strip_tags(htmlentities($data, ENT_QUOTES))), CMS::Singleton()->time()));
			//database::Singleton()->sql_update_by_id($this->id, $table, $fields = array(), $values = array())
			database::Singleton()->sql_update_or_insert('tbl_elements_draft', array('element_id', 'user_id'), array($this->id, User::Singleton()->id()), array('user_id', 'element_id', 'data', 'ts', 'title'), array(User::Singleton()->id(), $this->id, $built_xml, CMS::Singleton()->time(), $title));
//database::Singleton()->sql_update_or_insert('tbl_elements_draft', array('element_id', 'user_id'), array($this->id, User::Singleton()->id()), array('user_id', 'element_id', 'data', 'ts', 'title'), array(User::Singleton()->id(), $this->id, $xml, CMS::Singleton()->time(), $title));
			die('SAVED');
		}
		else
		{
			database::Singleton()->sql_delete_where('tbl_elements_draft', array('element_id', 'user_id'), array($this->id, user::Singleton()->id()));
			if(Security::Singleton()->can_approve_element())
			{
				if($this->process_save($built_xml, $title));
				{
					return true;
				}
			}
			else
			{
				if($this->submit_save_for_approval($built_xml, $title));
				{
					return true;
				}
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
	{
		return strip_tags($input);
	}

	protected function process_save($xml, $title)
	{
		$this->save_upload();

		//TODO call search code
//		$search = $this->generate_search($xml);
		//TODO Archiving
		$this->archive_data($xml);
		database::Singleton()->sql_update_by_id($this->id, 'tbl_elements', array('edited', 'editor'), array(time(), User::Singleton()->id()));
		database::Singleton()->sql_update_by_id($this->id, 'tbl_elements_data', array('data', 'title', 'search'), array($xml, $title, $this->generate_search($xml)));

		return true;
	}

	protected function submit_save_for_approval($xml, $title)
	{
		database::Singleton()->sql_update_or_insert('tbl_elements_approve', array('element_id', 'user_id'), array($this->id, User::Singleton()->id()), array('user_id', 'element_id', 'data', 'ts', 'title'), array(User::Singleton()->id(), $this->id, $xml, CMS::Singleton()->time(), $title));
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
				$this->element->setAttribute('approved', $this->approved);
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
		return common::node_from_xml($this->data, '/data/html');
	}

	//Over right this function in each element to transform the data XML to what ever needs to be sent to the XSLT processor
	protected function transform_data()
	{
		//$this->data = $this->data;
	}

	protected function get_data($data = NULL)
	{
		if($this->id != 'blank')
		{
			$approvalData = database::Singleton()->sql_select_first(array('title', 'data'), 'tbl_elements_approve', array('element_id'), array($this->id));

			if($data == NULL)
			{
				$db = Database::Singleton()->sql_select_first(array('data'), 'tbl_elements_data', array('id'), array($this->id), NULL, NULL, options::Singleton()->get_option('admin_mode'));
				$data = $db['data'];
			}

			$dbxml = '<data>'.$data.'</data>';

			if(options::Singleton()->get_option('admin_mode'))
			{
				$built_xml = '<data>';
				foreach($this->tags as $key => $value)
				{
					//If can approve then check if there outstanding approvals waiting or
					//Check if the user has any outstanding approvals waiting
					$user_awaiting_approval = Database::Singleton()->sql_select_first(array('id'), 'tbl_elements_approve', array('element_id', 'user_id'), array($this->id, user::Singleton()->id()), NULL, NULL, TRUE);
					$user_draft = Database::Singleton()->sql_select_first(array('ts'), 'tbl_elements_draft', array('element_id', 'user_id'), array($this->id, user::Singleton()->id()), NULL, NULL, TRUE);

					if(Security::Singleton()->can_approve_element($this->id) )
					{
						$can_approve = Database::Singleton()->sql_select_first(array('ts'), 'tbl_elements_approve', array('element_id'), array($this->id), NULL, NULL, TRUE);
					}

					if(isset($can_approve) or (isset($user_awaiting_approval)) )
					{
						$this->approve = true;
					}

					if(isset($user_draft['ts']))
					{
						$this->draft = $user_draft['ts'];
					}
					
					switch ($key)
					{
						case 'title':
							$data = Database::Singleton()->sql_select_first(array('visible', 'deleted', 'created', 'creator', 'edited', 'editor', 'approved', 'approver'), 'tbl_elements', array('id'), array($this->id), NULL, NULL, TRUE);
	
							//this is a bit of a hack and should probably be cleaned up at some piont
							$draft_title = database::Singleton()->sql_select_first(array('title'), 'tbl_elements_draft', array('element_id', 'user_id'), array($this->id, user::Singleton()->id()));
							if ($draft_title) {
								$title = $draft_title;
							}
							else{
								$title = Database::Singleton()->sql_select_first(array('title'), 'tbl_elements_data', array('id'), array($this->id));
							}
	
							$this->visible = $data['visible'];
							$this->deleted = $data['deleted'];
							$this->created = $data['created'];
							$this->creator = $data['creator'];
							$this->edited = $data['edited'];
							$this->editor = $data['editor'];
							$this->approved = $data['approved'];
							$this->aprover = $data['aprover'];
							$data = $approvalData?$approvalData['title']:$data = $title['title'];
						break;
						
					default:
						$data = common::node_from_xml($dbxml, '/data/'.$key);
					}
					
					if($value == 'html') {
						if ($approvalData) {
							$approvalData = common::node_from_xml($approvalData['data'], '/'.$key);
							$diffData = diffCompare::get_diff($data, $approvalData);
							$diffData = diffcompare::format_diff($diffData);
							if ($diffData && trim($diffData) != '') {
								$data = $diffData;
							}
						}
						$data = Common::html($data);
					}
					$data = utf8_decode(strip_tags($data));
					$built_xml .= '<'.$key.' type="'.$value.'">'.$data.'</'.$key.'>';
				}
				$built_xml .= '</data>';
				//die($built_xml);
				$this->data = $built_xml;
			}
			else
			{
				$this->data = $dbxml;
			}


			$this->search = $db['search'];
		}
		else
		{
			$this->data = '<data>Blank</data>';
			$this->search = NULL;
		}
	}

	protected function get_revision()
	{
		$this->revision = Database::Singleton()->sql_count_where('tbl_elements_data_archive', 'element_id', $this->id, options::Singleton()->get_option('admin_mode'));
		return $this->revision;
	}

	protected function get_revision_history(){
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

	protected function add_header()
	{

	}

	protected function attach_extra_data()
	{
		return array();
	}

}
?>