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

function get_permission_types($array){
	$types = array();

	foreach ($array as $type => $data){
		$types[] = $type;
	}
	unset($type, $data);

	$types[] = 'remove';

	return $types;
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

if($securityData['data'] == NULL){
	$permissions = security::singleton()->default_security();
}
else {
	$permissions = json_decode($securityData['data'], TRUE);
}
$permissions = $permissions['security']; //get rid of the top level array, which is only there for xml reasons

$types = get_permission_types($permissions);


####################
# save posted data #
####################

if (post::variable('savePerm')) {

	foreach ($_POST as $member => $new_perm){
		if ($new_perm != 'Save') {
			$member_data = explode('_', $member); //index 0 == group/user, index 1 == id

			//find and delete the user/group from old permission
			foreach ($permissions as &$permission){
				$user_key = array_search($member_data[1], $permission[$member_data[0]]);
				if ($user_key !== FALSE) {
					unset($permission[$member_data[0]][$user_key]);
					break;
				}
			}
			unset($permission);

			if ($new_perm != 'remove') {
				//assign the user to new permission and reset the array keys for json compliance
				$permissions[$new_perm][$member_data[0]][] = $member_data[1];
				$permissions[$new_perm][$member_data[0]] = array_values($permissions[$new_perm][$member_data[0]]);
			}
		}
	}
	unset($member, $new_perm);

	database::Singleton()->sql_update_by_col('tbl_security', array('page_id'), array($pageId), array('data'), array(json_encode(array('security' => $permissions))));
}
else if (post::variable('addUser')) {

	//get the users/lists to add
	if (post::variable('userList'))
		$userList = post::variable('userList');
	else{
		if (post::variable('groupType')) {
			$user = database::Singleton()->sql_select_first(array('id'), 'tbl_users', array('username'), array(post::variable('userSearch')));
			$userList = 'user_'.$user['id'];
		}
		else{
			$user = database::Singleton()->sql_select_first(array('id'), 'tbl_list_data', array('name'), array(post::variable('userSearch')));
			$userList = 'group_'.$user['id'];
		}
	}

	if (is_array($userList)) {
		foreach ($userList as $user){
			$user_details = explode('_', $user); //index 0 == user/group, index 1 == id
			$permissions[$types[0]][$user_details[0]][] = $user_details[1];
			array_unique($permissions[$types[0]][$user_details[0]]); //ensure the user exists only once
		}
		unset($user);
	}

	//save the new security in the DB
	database::Singleton()->sql_update_or_insert('tbl_security', array('page_id'), array($pageId), array('page_id', 'data'), array($pageId, json_encode(array('security' => $permissions))));
}


#######################
# construct html form #
#######################

$radioTemplate = '<label for="***NAME******TYPE***">***TYPE***</label><input type="radio" name="***NAME***" id="***NAME******TYPE***" value="***TYPE***" ***CHECKED*** />';

$groupDivs = array();

foreach ($permissions as $permission => $data){
	foreach ($data as $group => $members){
		if (!empty($members)) {
			if (!isset($groupDivs[$group])) {
				$groupDivs[$group][] = ucfirst($group).' permissions:<div id="'.$group.'">';
			}

			foreach ($members as $member){
				if ($group == 'user')
					$name = $users[find_user_by_id($member, $users)]['username'];
				else if ($group == 'group')
					$name = $lists[find_user_by_id($member, $lists)]['name'];

				$groupDivs[$group][] = '<div><strong>'.$name.':</strong> ';
				$userTemplate = str_replace('***NAME***', $group.'_'.$member, $radioTemplate);
				foreach ($types as $type){
					if (!strcasecmp($permission, $type)) {
						$finalTemplate = str_replace('***CHECKED***', 'checked', $userTemplate);
					}
					else $finalTemplate = str_replace('***CHECKED***', '', $userTemplate);
					$groupDivs[$group][] =  str_replace('***TYPE***', $type, $finalTemplate);
				}
				unset($type);
				$groupDivs[$group][] = '</div>';
			}
			unset($member);
		}
	}
	unset($group, $members);
}
unset($types, $data);

if(!empty($groupDivs)){
	foreach ($groupDivs as &$div){
		$div[] = '</div>';
		$div = implode('', $div);
	}
	$htmlList = '<div id="permList">
					<form method="post" action="">
						'.implode('', $groupDivs).'
						<input type="submit" name="savePerm" value="Save" />
					</form>
				 </div>';
}
else{
	$htmlList = '';
}

$userOptions = '';
if ($users) {
	$userList = array('<optgroup label="Users">');
	foreach ($users as $user){
		$userList[] = '<option value="user_'.$user['id'].'">'.$user['username'].'</option>';
	}
	$userList[] = '</optgroup>';
	$userOptions = implode('', $userList);
}

$listOptions = '';
if ($lists) {
	$listList = array('<optgroup label="Lists">');
	foreach ($lists as $list){
		$listList[] = '<option value="group_'.$list['id'].'">'.$list['name'].'</option>';
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