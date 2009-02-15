<?php
$path = get_include_path();
set_include_path  (dirname(__FILE__));
include_once 'Text/Diff.php';
include_once 'Text/Diff/Renderer.php';
include_once 'Text/Diff/Renderer/inline.php';
set_include_path($path);

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
		//$html_fragment = htmlentities($html_fragment);
   		return ( $html_fragment);
	}
}
/*$old = 'This is a text string';
$new = 'This is a diffrent text string';
$diff = diffcompare::get_diff($old, $new);
echo('Old: '.$old.'<br/>');
echo('New: '.$new.'<br/>');

echo('Diff: '.$diff.'<br/>');
echo('Formated: '.diffcompare::format_diff($diff).'<br/>')*/
?>