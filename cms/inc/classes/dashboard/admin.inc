<?php
$selected = false;
$dashboard_type = strtolower(basename(substr(__FILE__, 0, -4)));

if(cmspage_dashboard::type() != $dashboard_type){
	$html = 'Access to system tools';	
}
else {
	$selected = true;
	$html = '<ul>';
	foreach(glob(DIR_CLASSES.'cmspage'.DS.'tools'.DS.'admin'.DS.'*.inc', GLOB_NOSORT) as $filename) 
	{
		$name = substr(basename($filename), 0, -4);
		$html .= '<li><a href="'.Settings::singleton()->get_setting('document_root').'/tools/admin/'.$name.'">'.str_ireplace('_', ' ',$name).'</a></li>';
	}
	unset($name);
			
	$html .= '</ul>';
}

cmspage_dashboard::register('Administration', $dashboard_type, 'dashboard_admin', null, null, null, $html, $selected);	
unset($html);
unset($dashboard_type);