<?php

/**
*	The purpose of this file is to rename a node in the database 
*	
*	Input : $_GET['renameId']
*			$_GET['newName']
*
*		OR
*
*			$_GET['deleteIds'] 
*
*		If a delete request is sent
*
***/

/*  The TWO line below is only for the demo as we aren't connected to the database */
/*
echo "OK";
exit;


if(isset($_GET['renameId']) && isset($_GET['newName']))	{	// variables are set

	// Typical code
	
	$res = mysql_query("select * from category where ID='".$_GET['renameId']."'");
	if($inf = mysql_fetch_array($res)){
		mysql_query("update category set categoryName='".$_GET['newName']."' where ID='".$inf["ID"]."'") or die("NOT OK");
		echo "OK";	// OK when everything is ok
	}
	echo "NOT OK";	// Node didn't exist -> Message not ok


	
	exit;
}

if(isset($_GET['deleteIds'])){	// Format of $_GET['deleteIds'] : A comma separated list of ids to delete, example: 1,2,3,4,5,6,7
	
	// Typical code
	
	mysql_query("delete from category where ID IN(".$_GET['deleteIds']."')") or die("NOT OK");
	echo "OK";
}

echo "NOT OK";

*/
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
		document.myForm.elements['saveString'].value = treeObj.getNodeOrders();
		document.myForm.submit();		
	}
	</script>
</head>
<body>
<div>
<div>
<ul id="site_structure" class="dhtmlgoodies_tree">
	<li noDrag="true"><a href="#">1</a></li>
	<li><a href="#">2</a></li>
	<li><a href="#">3</a></li>
	<li><a href="#">4</a></li>
	<li><a href="#">5</a></li>
	<li><a href="#">6</a></li>
	<li><a href="#">7</a>
		<ul>
			<li><a href="#">6-1</a></li>
			<li><a href="#">6-2</a></li>
			<li><a href="#">6-3</a></li>
		</ul>
	</li>
</ul>
</div>
<script type="text/javascript">	
	treeObj = new JSDragDropTree();
	treeObj.setTreeId('site_structure');
	treeObj.setMaximumDepth(20);
	treeObj.setMessageMaximumDepthReached('Maximum depth reached'); // If you want to show a message when maximum depth is reached, i.e. on drop.
	treeObj.initTree();
	treeObj.expandAll();
	</script>
	<a href="#" onclick="treeObj.collapseAll()">Collapse all</a> | 
	<a href="#" onclick="treeObj.expandAll()">Expand all</a>
	<p style="margin:10px">Use your mouse to drag and drop the nodes. Use the "Save" button to save your changes.</p>
	<form id="save_site_structure" method="post" action="">
	<p>
		<input type="hidden" name="saveString" />
		<input type="button" onclick="saveMyTree()" value="Save">
	</p>
	</form>
</div>

</body>
</html>