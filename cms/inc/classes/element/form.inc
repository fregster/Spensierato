<?php
element::register('Form', 'element_Form', false, 'book', 'book_edit');
class element_Form extends element_Default
{
	public function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'Form';
		$this->allowed_tags(array('html' => 'html'));
	}

	/*
	 * Taken from content for a reference
	public function build_form($name = 'Form', $array, $values = null, $description = null, $post = true, $action = '', $return_xml = false){
		csrf::build_token();
		
		$xml = '<form><name>'.Common::strip_non_alpha_numeric($name).'</name><description>'.Common::strip_non_alpha_numeric($description).'</description><post>'.Common::strip_non_alpha_numeric($post).'</post>';
		foreach($array as $field => $type){
			$value = $type;
			if(is_array($type)){
				$value = '';
				foreach ($type as $option){
					$value .= '<option>'.Common::strip_non_alpha_numeric($option).'</option>';
				}
				$type = 'options';
			}
			$xml .= '<field type="'.Common::strip_non_alpha_numeric($type).'"><name>'.Common::strip_non_alpha_numeric($field).'</name><type>'.Common::strip_non_alpha_numeric($value).'</type></field>';
		}
		$xml .= '</form>';
		
		if($return_xml){
			return $xml;
		}
		
		$this->attach_content($xml);
	}
	*/
}