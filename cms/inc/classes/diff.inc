<?php
class string_compare
{
	function __construct()
	{
		require_once 'Text/Diff.php';
		require_once 'Text/Diff/Renderer.php';
		require_once 'Text/Diff/Renderer/inline.php';
	}
	
	public function show_diff($string1, $string2, $tidy_config = NULL)
	{
		//perform diff, print output
		$diff = &new Text_Diff($string1, $string2);
		$renderer = &new Text_Diff_Renderer_inline();
	
		$html_decoded = html_entity_decode($renderer->render($diff));	
	
		if($tidy_config == NULL)
		{
			$tidy_config = array('show-body-only' => true,
							'indent' => TRUE,
				           	'output-xhtml' => TRUE,
		             		'hide-comments' => TRUE,
							'join-styles' => TRUE,
				            'wrap' => 200);
		}
		$tidy = tidy_parse_string($html_decoded, $tidy_config, 'UTF8');
		$tidy->cleanRepair();
		return $tidy;
	}
}

?>