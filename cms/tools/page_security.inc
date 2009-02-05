<?php
//page security
if (!user::Singleton()->is_logged_in() || !Security::singleton()->can_approve()) common::bounce_to_referer();


#############
# functions #
#############

function find_user_by_id($id, $array){
	foreach ($array as $user){
		$key = array_search($id, $user);
		if (!is_null($key)) return $key;
	}
	return false;
}


##########################
# data init for the tool #
##########################

$pageId = currentPage::Singleton()->page_path();

//get all non-admin users
$users = database::Singleton()->sql_select(array('id', 'username'), 'tbl_users', array('admin'), array('0'));

//get all visible lists
$lists = database::Singleton()->sql_select(array('id', 'name'), 'tbl_list_data', array('visible'), array('1'));

//get permissions for this page
$securityData = database::Singleton()->sql_select_first(array('data'), 'tbl_security', array('page_id'), $pageId);

$securityXML = new SimpleXMLElement($securityData['data']);
unset($securityData);

$permissions = array(
					'user' => array(),
					'group' => array()
					);

$types = array();

//populate $pemissions with the security data
foreach ($securityXML->children() as $child){
	array_push($types, $child->getName());
	foreach ($child->children() as $element){
		if (strip_tags($element->asXML()) != ''){
			if ($element->getName() == 'user')
				array_push($permissions['user'][$child->getName()], strip_tags($element->asXML()));
			else if ($element->getName() == 'group')
				array_push($permissions['group'][$child->getName()], strip_tags($element->asXML()));
		}
	}
}
unset($child);


####################
# save posted data #
####################

$postVars = new post;

if ($postVars->variable('savePerm')) {

	foreach ($permissions as $group => $permissions){
		foreach ($permissions as $permission => $user){
			if ($permission != $postVars->variable($user)){
				$securityXML->$postVars->variable($user)->addChild($group, $postVars->variable($user));
				$securityXML->$permission = NULL;
			}
		}
		unset($permission, $user);
	}
	unset($group);

	$securityData = $securityXML->asXML();
	unset($securityXML);

	foreach ($permissions as $group => $value){
		$securityData = str_replace("<$group></$group>", '', $securityData);
		$securityData = str_replace("<$group/>", '', $securityData);
	}

	database::Singleton()->sql_update_by_id($pageId, 'tbl_security', array('data'), array($securityData));

	$securityXML = new SimpleXMLElement($securityData);
}
else if ($postVars->variable('addUser')) {

	//set the user as a user. default permissions is the first option in the list
	foreach ($securityXML->children() as $child){
		$child->addChild('user', $postVars->variable('userList'));
		break;
	}

	//convert xml object to string
	$securityData = $securityXML->asXML();

	//save the new security in the DB
	database::Singleton()->sql_update_by_id($pageId, 'tbl_security', array('data'), $securityData);

	unset($securityData, $securityXML);
}

unset($postVars);


#######################
# construct html form #
#######################

$radioTemplate = '<label for="***NAME******TYPE***">***TYPE***</label><input type="radio" name="***NAME***" id="***NAME******TYPE***" value="***TYPE***" ***CHECKED*** />';

$htmlArray = array('<div id="permList"><form method="post" action="">');

foreach ($permissions as $group => $permissions){
	array_push($htmlArray, $group.' permissions:', '<p id="'.$group.'">');
	foreach ($permissions as $permission => $user){
		$username = $users[find_user_by_id($user, $array)];
		$htmlArray[] = '<div>'.$username.': ';
		$userTemplate = str_replace('***NAME***', $user, $radioTemplate);
		foreach ($types as $type){
			if ($permission == $type) $userTemplate = str_replace('***CHECKED***', 'checked', $userTemplate);
			else $userTemplate = str_replace('***CHECKED***', '', $userTemplate);
			$htmlArray[] =  str_replace('***TYPE***', $type, $userTemplate);
		}
		unset($type);
		$htmlArray[] = '</div>';
	}
	unset($permission, $user);
	$htmlArray[] = '</p>';
}
unset($group);

array_push($htmlArray, '<input type="submit" name="savePerm" value="Save" />', '</form></div>');

if (count($permissions['user']) || count($permissions['group']))
	$htmlList = implode('', $htmlArray);


if ($usres) {
	$userList = array('<optgroup label="Users">');
	foreach ($users as $user){
		$userList[] = '<option value="'.$user['id'].'">'.$user['username'].'</option>';
	}
	$userList[] = '</optgroup>';
	$userOptions = implode('', $userList);
}

if ($lists) {
	$listList = array('<optgroup label="Lists">');
	foreach ($lists as $list){
		$listList[] = '<option value="'.$list['id'].'">'.$list['name'].'</option>';
	}
	$listList[] = '</optgroup>';
	$listOptions = implode('', $listList);
}

$addUserHTML = <<<HTMLBLOCK
<div id="findUser">
	<form methos="post" action="">
		<div>Add users and groups:</div>
		<div>
			<select name="userList" multiple>
				$userOptions.$listOptions
			</select>
		</div>
		<input type="submit" name="addUser" value="Add" />
	</form>
</div>
HTMLBLOCK;

die($htmlList.$addUserHTML);
?>