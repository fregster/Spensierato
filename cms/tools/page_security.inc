<?php
//page security
if (!user::Singleton()->is_logged_in() || !Security::singleton()->can_approve()) common::bounce_to_referer();


#############
# functions #
#############

function find_user_by_id($id, $array){
	foreach ($array as $key => $user){
		if ($user['id'] == $id)
			return $key;
	}
}

function clean_xml($xml){
	$xml = trim(preg_replace('#<\?.*\?>#', '', $xml));
	$xml = preg_replace('#(<user/>|<group/>)#', '', $xml);
	return $xml;
}

function get_permission_types($xml){
	$types = array();

	$xmlObj = new SimpleXMLElement($xml);
	foreach ($xmlObj->children() as $child){
		$types[] = $child->getName();
	}
	unset($child, $xmlObj);

	$types[] = 'remove';

	return $types;
}

function update_permissions($xml){
	$permissions = array(
					'user' => array(),
					'group' => array()
					);

	$xmlObj = new SimpleXMLElement($xml);

	//populate $pemissions with the security data
	foreach ($xmlObj->children() as $child){
		foreach ($child->children() as $element){
			if (strip_tags($element->asXML()) != ''){
				if ($element->getName() == 'user')
					$permissions['user'][strip_tags($element->asXML())] = $child->getName();
				else if ($element->getName() == 'group')
					$permissions['group'][strip_tags($element->asXML())] = $child->getName();
			}
		}
	}
	unset($child, $xmlObj);

	return $permissions;
}

function change_user_permission($userId, $group, $permLvl, $xmlStr){

	$dom = new DOMDocument();
	$dom->loadXML($xmlStr);

	//first remove the user from the old permission
	$x = new DomXPath($dom);
	$userList=$x->query("//{$group}[text()='{$userId}']");

	foreach ($userList as $domElement){
		$domNode = $dom->importNode($domElement, true);
		$parent = $domNode->parentNode;
		$parent->removeChild($domNode);
	}

	//now place it in the new permission
	if ($permLvl != 'remove') {
		$x = new DomXPath($dom);
		$permissionList=$x->query("//{$permLvl}");

		foreach ($permissionList as $domElement){
			$permissionNode = $dom->importNode($domElement, true);
			foreach ($userList as $domElement){
				$domNode = $dom->importNode($domElement, true);
				$permissionNode->appendChild($domNode);
			}
		}
		unset($domElement);
	}

	return $dom->saveXML();
}


##########################
# data init for the tool #
##########################

$pageId = CMS::Singleton()->path_vars(2);

//get all non-admin users
$users = database::Singleton()->sql_select(array('id', 'username'), 'tbl_users', array('admin'), array('0'));

//get all visible lists
$lists = database::Singleton()->sql_select(array('id', 'name'), 'tbl_list_data');

//get permissions for this page from DB
$securityData = database::Singleton()->sql_select_first(array('data'), 'tbl_security', array('page_id'), array($pageId));

if(is_null($securityData['data'])){
	$securityData['data'] = security::singleton()->default_security_xml();
}

$types = get_permission_types($securityData['data']);
$permissions = update_permissions($securityData['data']);


####################
# save posted data #
####################

$postVars = new post;

if ($postVars->variable('savePerm')) {

	$xmlStr = $securityData['data'];

	foreach ($permissions as $group => $allUsers){
		foreach ($allUsers as $user => $permission){
			if ($permission != $postVars->variable($user)){
				$xmlStr = change_user_permission($user, $group, $postVars->variable($user), $xmlStr);
			}
		}
		unset($user, $permission);
	}
	unset($group, $allUsers);

	$securityData = clean_xml($xmlStr);

	database::Singleton()->sql_update_by_col('tbl_security', array('page_id'), array($pageId), array('data'), array($securityData));

	//update permissions so the html form is updated
	$permissions = update_permissions($securityData);
}
else if ($postVars->variable('addUser')) {

	$securityXML = new SimpleXMLElement($securityData['data']);

	//get the users/lists to add
	if ($postVars->variable('userList'))
		$userList = $postVars->variable('userList');
	else{
		if ($postVars->variable('groupType')) {
			$user = database::Singleton()->sql_select_first(array('id'), 'tbl_users', array('username'), array($postVars->variable('userSearch')));
			$userList = 'user-'.$user['id'];
		}
		else{
			$user = database::Singleton()->sql_select_first(array('id'), 'tbl_list_data', array('name'), array($postVars->variable('userSearch')));
			$userList = 'group-'.$user['id'];
		}
	}

	//get the users/lists to be added from post data
	$userArray = array();
	if (is_array($userList)) {
		foreach ($userList as $user){
			$postValues = explode('-', $user); //index 0 = group, index 1 = user
			//only add users that don't exist
			if (!isset($permissions[$postValues[0]][$postValues[1]]))
				$userArray[$postValues[1]] = $postValues[0];
		}
		unset($user);
	}
	else{
		$postValues = explode('-', $userList);
		//only add users that don't exist
		if (!isset($permissions[$postValues[0]][$postValues[1]]))
			$userArray[$postValues[1]] = $postValues[0];
	}
	unset($postValues, $userList);

	//set the user as a user. default permissions is the first option in the list
	foreach ($userArray as $user => $group){
		foreach ($securityXML->children() as $child){
			$child->addChild($group, $user);
			break;
		}
	}

	//convert xml object to string and drop the xml header
	$securityData = clean_xml($securityXML->asXML());

	//save the new security in the DB
	database::Singleton()->sql_update_or_insert('tbl_security', array('page_id'), array($pageId), array('page_id', 'data'), array($pageId, $securityData));

	//update permissions so the form is updated
	$permissions = update_permissions($securityData);
}

unset($postVars, $securityData, $securityXML);


#######################
# construct html form #
#######################

$radioTemplate = '<label for="***NAME******TYPE***">***TYPE***</label><input type="radio" name="***NAME***" id="***NAME******TYPE***" value="***TYPE***" ***CHECKED*** />';

$htmlArray = array('<div id="permList"><form method="post" action="">');

foreach ($permissions as $group => $allUsers){
	array_push($htmlArray, $group.' permissions:', '<div id="'.$group.'">');
	foreach ($allUsers as $user => $permission){
		if ($group == 'user')
			$name = $users[find_user_by_id($user, $users)]['username'];
		else if ($group == 'group')
			$name = $lists[find_user_by_id($user, $lists)]['name'];

		$htmlArray[] = '<div><strong>'.$name.':</strong> ';
		$userTemplate = str_replace('***NAME***', $user, $radioTemplate);
		foreach ($types as $type){
			if (!strcasecmp($permission, $type)) {
				$finalTemplate = str_replace('***CHECKED***', 'checked', $userTemplate);
			}
			else $finalTemplate = str_replace('***CHECKED***', '', $userTemplate);
			$htmlArray[] =  str_replace('***TYPE***', $type, $finalTemplate);
		}
		unset($type);
		$htmlArray[] = '</div>';
	}
	unset($permission, $user);
	$htmlArray[] = '</div>';
}
unset($group, $allUsers);

array_push($htmlArray, '<input type="submit" name="savePerm" value="Save" />', '</form></div>');

if (count($permissions['user']) || count($permissions['group']))
	$htmlList = implode('', $htmlArray);


if ($users) {
	$userList = array('<optgroup label="Users">');
	foreach ($users as $user){
		$userList[] = '<option value="user-'.$user['id'].'">'.$user['username'].'</option>';
	}
	$userList[] = '</optgroup>';
	$userOptions = implode('', $userList);
}

if ($lists) {
	$listList = array('<optgroup label="Lists">');
	foreach ($lists as $list){
		$listList[] = '<option value="group-'.$list['id'].'">'.$list['name'].'</option>';
	}
	$listList[] = '</optgroup>';
	$listOptions = implode('', $listList);
}

$addUserHTML = <<<HTMLBLOCK
<div id="findUser">
	<form method="post" action="">
		<table cellspacing="20" cellpadding="0">
			<tr>
				<td>
					<div>Add users and groups from list:</div>
					<div>
						<select name="userList[]" multiple>
							$userOptions.$listOptions
						</select>
					</div>
				</td>
				<td>OR</td>
				<td>
					<div>Add users and groups from search:</div>
					<div><select name="groupType"><option value="1">User</option><option value="0">List</option></select>
					<div><input type="text" name="userSearch" /></div
				</td
			</tr>
			<tr>
				<td colspan="3" align="center"><input type="submit" name="addUser" value="Add" /></td>
			</tr>
		</table>
	</form>
</div>
HTMLBLOCK;

die($htmlList.$addUserHTML);
?>