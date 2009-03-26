<?php
$path = get_include_path();
set_include_path  ($path. PATH_SEPARATOR .dirname(__FILE__));
/*
include_once 'Text/Diff.php';
include_once 'Text/Diff/Renderer.php';
include_once 'Text/Diff/Renderer/inline.php';

class diffCompare
{
	public function show_diff_as_html($old, $new, $tidy_config = NULL)
	{

		$diff = diffCompare::get_diff($old, $new);
		$html = '<html><head><title>Diff</title></head><body>'.html_entity_decode($diff).'  </body></html>';

		if(extension_loaded('tidy'))
		{
			if($tidy_config == NULL)
			{
				$tidy_config = array('show-body-only' => true,
					           	'output-xhtml' => TRUE,
			             		'hide-comments' => TRUE,
								'join-styles' => TRUE,
								'join-classes' => TRUE,
								'drop-proprietary-attributes' => TRUE,
								'hide-comments' => TRUE,
								'drop-empty-paras' => TRUE,
								'word-2000' => TRUE,
					            'wrap' => 200);
			}
			$tidy = tidy_parse_string($html_decoded, $tidy_config, 'UTF8');
			$tidy->cleanRepair();
			$html = tidy_get_output($tidy);
		}
		return $html;
	}

	public function get_diff($old, $new)
	{
		if(!is_array($old))
		{
			$old = array($old);
		}

		if(!is_array($new))
		{
			$new = array($new);
		}
		$diff = &new Text_Diff($old, $new);
		$renderer = &new Text_Diff_Renderer_inline();

		return $renderer->render($diff);
	}

	public function format_diff($diff)
	{
		$dom_temp = new DOMDocument();
		$dom_temp->loadHTML($diff);
 		$xpath = new DOMXPath($dom_temp);

   		foreach($xpath->query('//ins') as $ins)
   		{
   			$ins->setAttribute('style', 'color: green;');
   		}
		foreach($xpath->query('//del') as $del)
   		{
   			$del->setAttribute('style', 'color: red;');
   		}

   		$html_fragment = preg_replace('/^<!DOCTYPE.+?>/', '', str_replace( array('<html>', '</html>', '<body>', '</body>'), array('', '', '', ''), $dom_temp->saveHTML()));
		return ( $html_fragment);
	}
}*/

	// This line must be used to load the Text/Diff.php PEAR module
	// This can be downloaded from http://pear.php.net/package/Text_Diff
	require_once("Text/Diff.php");

// Object created like this:
// $diff = new diff($from_text,$to_text);
// $from_text is the original text
// $to_text is the modified text
class diffCompare extends Text_MappedDiff {

	public function __construct($from_text,$to_text) {
		#public function __construct($from_text,$to_text) {

		// Split the text into words
		//$from_words = preg_split('/\b/',$from_text);
		//$to_words = preg_split('/\b/',$to_text);

		// These alternate split statements will split the text into individual characters
		$from_words = str_split($from_text,1);
		$to_words = str_split($to_text,1);

		// These split statements will split the text into lines
		//$from_words = explode("\n",$from_text);
		//$to_words = explode("\n",$to_text);

		// Text_MappedDiff requires copies of these variables
		$mapped_from_words = $from_words;
		$mapped_to_words = $to_words;

		// Call the construct method of the parent class
		parent::Text_MappedDiff($from_words,$to_words,$mapped_from_words,$mapped_to_words);
	}

	// Renders the difference
	public function render() {
		$difference = $this->getDiff(); // Calls the getDiff() method, part of Text_MappedDiff
		$render = ''; // This will be the rendered difference.
		foreach ($difference as $op) {
			// $op is the operation getDiff() has calculated.
			// You can tell which operation (add, delete or change) $op is by checking which class the object is
			#if (is_a(&$op,'Text_Diff_Op_copy')) {
				if ($op instanceof Text_Diff_Op_copy) { // This uses the instanceof operator which is only in PHP5+, in PHP4 you can use is_a()
				// Text was copied (not changed), add it to the rendered difference unmodified
				$render .= implode('',$op->final);
			#} elseif (is_a(&$op,'Text_Diff_Op_delete')) {
				} elseif ($op instanceof Text_Diff_Op_delete) {
				// Text was deleted, add it to the rendered difference in red
					$temp = implode('',$op->orig);
					$temp = str_replace(htmlentities('</p><p>'), html_entity_decode('</span></p><p><span style="background-color: #FF0000;">'), $temp);
				$render .= '<span style="background-color: #FF0000;">' . $temp . '</span>';
			#} elseif (is_a(&$op,'Text_Diff_Op_add')) {
				} elseif ($op instanceof Text_Diff_Op_add) {
				// Text was added, add it to the rendered difference in green
				$temp = implode('',$op->final);
				$temp = str_replace(htmlentities('</p><p>'), html_entity_decode('</span></p><p><span style="background-color: #00FF00;">'), $temp);
				$render .= '<span style="background-color: #00FF00;">' . $temp . '</span>';
			#} elseif (is_a(&$op,'Text_Diff_Op_change')) {
				} elseif ($op instanceof Text_Diff_Op_change) {
				// Text was modified, add it to the rendered difference as if the original text was deleted and the new text was added
				$temp = implode('',$op->orig);
				$temp = str_replace(htmlentities('</p><p>'), html_entity_decode('</span></p><p><span style="background-color: #FF0000;">'), $temp);
				$render .= '<span style="background-color: #FF0000;">' . $temp . '</span>';
				$temp = implode('',$op->final);
				$temp = str_replace(htmlentities('</p><p>'), html_entity_decode('</span></p><p><span style="background-color: #00FF00;">'), $temp);
				$render .= '<span style="background-color: #00FF00;">' . $temp . '</span>';
			}
		}

		return $render;
	}

}

?>