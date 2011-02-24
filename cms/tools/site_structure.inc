<?php
if (!user::Singleton()->is_logged_in() || !user::Singleton()->is_admin()) { common::redirect('home'); }
/*
 * This is some vial code but as a quick get the job done effort.
 * TODO:Rewrite this whole script as I think is *****
*/
$update = false;
if(strtolower($_SERVER['REQUEST_METHOD']) == 'post'){
	$saveString = explode(',',post::variable('saveString'));
	$menu = menu::singleton()->get_full_menu(true);
	$curentMenu = array();
	
	//flatten the current menu array
	foreach($menu as $item){
		$currentMenu[$item['id']] = $item['parent_id'];
	}
	
	unset($menu);
	
	///Update any changes
	$newMenu = array();
	foreach($saveString as $menuItem){
		$parts = explode('-', $menuItem);
		if($currentMenu[$parts[0]] != $parts[1] && security::singleton()->can_edit($parts[0]) && security::singleton()->can_edit($parts[1])){
			//Build list of changes
			$newMenu[$parts[0]] = (int)$parts[1];
			$update = true;
			if(post::variable('save', 'isset')){
				$updateString .= 'Updated Page: '.$parts[0];
				Database::Singleton()->sql_update_or_insert('tbl_pages', array('id'), array($parts[0]), array('parent_id', 'edited', 'editor'), array($parts[1], CMS::$time, user::Singleton()->id()));
			}
		}
	}
	
	unset($saveString);
	unset($currentMenu);
}

?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html>
<head>
	<title>Site Menu Editor</title>
	<script type="text/javascript">var document_root = '<?php echo(settings::singleton()->get_setting('document_root')); ?>';</script>
	<script type="text/javascript" src="<?php echo(settings::singleton()->get_setting('document_root')); ?>/js/folder_tree/ajax.js"></script>
	<script type="text/javascript" src="<?php echo(settings::singleton()->get_setting('document_root')); ?>/js/folder_tree/context-menu.js"></script><!-- IMPORTANT! INCLUDE THE context-menu.js FILE BEFORE drag-drop-folder-tree.js -->
	<script type="text/javascript" src="<?php echo(settings::singleton()->get_setting('document_root')); ?>/js/folder_tree/drag-drop-folder-tree.js"></script>
	<link rel="stylesheet" href="<?php echo(settings::singleton()->get_setting('document_root')); ?>/skin/menu_editor.css" type="text/css"></link>
	<style type="text/css">
	img{
		border:0px;
	}
	</style>
	<script type="text/javascript">
	//--------------------------------
	// Save functions
	//--------------------------------
	var ajaxObjects = new Array();
	
	// Use something like this if you want to save data by Ajax.
	function saveMyTree()
	{
			saveString = treeObj.getNodeOrders();
			var ajaxIndex = ajaxObjects.length;
			ajaxObjects[ajaxIndex] = new sack();
			var url = 'saveNodes.php?saveString=' + saveString;
			ajaxObjects[ajaxIndex].requestFile = url;	// Specifying which file to get
			ajaxObjects[ajaxIndex].onCompletion = function() { saveComplete(ajaxIndex); } ;	// Specify function that will be executed after file has been found
			ajaxObjects[ajaxIndex].runAJAX();		// Execute AJAX function			
		
	}
	function saveComplete(index)
	{
		alert(ajaxObjects[index].response);			
	}

	
	// Call this function if you want to save it by a form.
	function saveMyTree_byForm()
	{
		document.forms.save_site_structure.elements['saveString'].value = treeObj.getNodeOrders();
		document.forms.save_site_structure.submit();		
	}
	</script>
</head>
<body>
<?php 
if($update && !post::variable('save', 'isset')){
	$saveString = '';
	foreach($newMenu as $key => $value){
		$saveString .= $key.'-'.$value.',';
		echo 'Page: '.$key.' new parent: '.$value.'<br/>';
	}
	
	echo '<div><form method="post" action="" id="comit">';
	echo '<input type="hidden" name="saveString" id="saveString" value="'.$saveString.'" />';
	echo '<input type="hidden" name="save" id="save" value="true" />';
	echo '<br/><input type="submit" value="Comit these changes"/></form><br/></div>';
}
?>
<div>
<div>

<?php 
$menu_array = array();
$xml = simplexml_load_string(xml::array_to_xml(menu::singleton()->get_full_menu(true)));
$parent_ids = $xml->xpath("//parent_id");
foreach($parent_ids as $parent){
	$does_parent_exsist = $xml->xpath('//item[id = '.(int)$parent.']');
	if(!$does_parent_exsist){
		$menu_array[(int)$parent] = (int)$parent;
	}
}

foreach($menu_array as $top_menu_item){
	$menu_array[$top_menu_item] = build_menu_array($top_menu_item, $xml);
	$menu_array[$top_menu_item] = $menu_array[$top_menu_item][$top_menu_item];
}

function build_menu_array($parent_id, $xml)
{	
	$array = array();
	$results = $xml->xpath("//item[parent_id = $parent_id]");
	
	if(count($results) > 0)
	{
		foreach($results as $result){
			$array[$parent_id][] = array('id' => (int)$result->id, 'title' => (string)$result->title, 'children' => build_menu_array((int)$result->id, $xml));
		}
	}

//	array_merge($array);
	return $array;
}

unset($xml);
unset($parent_ids);

function build_html($menu)
{
	if(is_array($menu)){
		foreach($menu as $item){
			//'.settings::singleton()->get_Setting('document_root').'/'.$item['id'].'
			if(strlen($item['title']) < 1){ $item['title'] = 'Untitled page'; }
			echo('<li id="li_'.$item['id'].'"><a href="#">'.$item['title'].'</a>');
			if(count($item['children']) > 0){
				$keys = array_keys($item['children']);
				echo '<ul>';
				build_html($item['children'][$keys[0]]);
				echo '</ul>'.PHP_EOL;
			}
			echo '</li>'.PHP_EOL;
		}
	}
}
echo '<ul id="site_structure" class="dhtmlgoodies_tree">';
foreach($menu_array as $root){ 
	build_html($root);
}
echo '</ul>';
?>
</div>
<script type="text/javascript">	
	treeObj = new JSDragDropTree();
	treeObj.setTreeId('site_structure');
	treeObj.setMaximumDepth(100);
	treeObj.setMessageMaximumDepthReached('Maximum depth reached'); // If you want to show a message when maximum depth is reached, i.e. on drop.
	treeObj.initTree();
	treeObj.collapseAll();
	</script>
	<a href="#" onclick="treeObj.collapseAll()">Collapse all</a> | 
	<a href="#" onclick="treeObj.expandAll()">Expand all</a>
	<p style="margin:10px">Use your mouse to drag and drop the nodes. Use the "Save" button to save your changes.</p>
	<form id="save_site_structure" method="post" action="">
	<p>
		<input type="hidden" name="saveString" id="saveString"/>
		<input type="button" onclick="saveMyTree_byForm()" value="Save">
	</p>
	</form>
</div>

</body>
</html>