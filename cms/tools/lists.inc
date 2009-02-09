<?php
//page security
if (!user::Singleton()->is_logged_in() || !Security::singleton()->can_approve()) common::bounce_to_referer();

function find_user_by_id($id, $id_col, $array){
	foreach ($array as $key => $user){
		if ($user[$id_col] == $id)
			return $key;
	}
	return false;
}

##########################
# data init for the tool #
##########################

$lists = database::Singleton()->sql_select(array('id', 'name'), 'tbl_list_data');

$activeList = $lists[0]['id'];
$members = database::Singleton()->sql_select(array('user_id'), 'tbl_list_membership', array('list_id'), array($activeList));

$users = database::Singleton()->sql_select(array('id', 'username'), 'tbl_users', array('admin'), array(0));

$finalHTML = <<<HTMLBLOCK
<p>
	<a href="{$GLOBALS['document_root']}/tools/lists/new"><button>Create New List</button></a>
	<a href="{$GLOBALS['document_root']}/tools/lists/edit"><button>Edit Existing List</button></a>
</p>
HTMLBLOCK;


####################
# save posted data #
####################

$postVars = new post;

if ($postVars->variable('createList')) {

	if ($postVars->variable('listName')) {
		$checkNameExists = database::Singleton()->sql_select_first(array('id'), 'tbl_list_data', array('name'), array($postVars->variable('listName')));

		if (!$checkNameExists) {

			CMS::Singleton()->path_vars[2] = 'edit';
			$insertFields = array('name', 'valid_from', 'valid_to', 'created', 'created_by');

			$insertValues = array($postVars->variable('listName'));
			$insertValues[] = ($postVars->variable('fromDate')?$postVars->variable('fromDate'):time());
			$insertValues[] = ($postVars->variable('toDate')?$postVars->variable('toDate'):'0');
			$insertValues[] = time();
			$insertValues[] = user::Singleton()->id();

			$list_id = database::Singleton()->sql_insert('tbl_list_data', $insertFields, $insertValues);

			if ($postVars->variable('addUsers')) {
				foreach ($postVars->variable('addUsers') as $user){
					database::Singleton()->sql_insert('tbl_list_membership', array('list_id', 'user_id'), array($list_id, $user));
				}
				unset($user);
			}
		}
		else $finalHTML .= '<div>Error: A list with that name already exists.</div>';
	}
	else $finalHTML .= '<div>Error: You forgot to provide a list name.</div>';
}
else if($postVars->variable('saveUsers')){
	if ($postVars->variable('removeUsers'))
		database::Singleton()->sql_delete_where('tbl_list_membership', array('user_id'), array($postVars->variable('removeUsers')));

	if ($postVars->variable('addUsers')) {
		foreach ($postVars->variable('addUsers') as $user){
			database::Singleton()->sql_insert('tbl_list_membership', array('list_id', 'user_id'), array($postVars->variable('activeList'), $user));
		}
		unset($user);
	}
}
else if ($postVars->variable('changeList')) {
	$activeList = $postVars->variable('chooseList');
	$members = database::Singleton()->sql_select(array('user_id'), 'tbl_list_membership', array('list_id'), array($activeList));
}

#######################
# construct html form #
#######################

$nonMembersList = array();
foreach ($users as $user){
	if ((find_user_by_id($user['id'], 'user_id', $members) === false) || (CMS::Singleton()->path_vars[2] != 'edit'))
		$nonMembersList[] = '<option value="'.$user['id'].'">'.$user['username'].'</option>';
}
unset($user);
$nonMembersList = implode('', $nonMembersList);

if (CMS::Singleton()->path_vars[2] == 'edit') {
	if ($lists) {

		$listList = array();
		$membersList = array();

		foreach ($lists as $list){
			$listList[] = '<option value="'.$list['id'].'">'.$list['name'].'</option>';
		}
		unset($list);
		$listList = implode('', $listList);

		foreach ($members as $member){
			$membersList[] = '<option value="'.$member['user_id'].'">'.$users[find_user_by_id($member['user_id'], 'id', $users)]['username'].'</option>';
		}
		unset($member);
		$membersList = implode('', $membersList);

		$listForm = <<<HTMLBLOCK
		<form method="post" action="">
			<div>
				Select a list to edit:
				<select name="chooseList">
					$listList
				</select>
			</div>
			<input type="submit" name="changeList" value="Change" />
		</form>
HTMLBLOCK;

		$userControlForm = <<<HTMLBLOCK
		<form method="post" action="">
			<table>
				<tr>
					<td>
						<div>Select users to remove from the list:</div>
						<select name="removeUsers[]" multiple>
							$membersList
						</select>
					</td>
					<td>
						<div>Select users to add to the list:</div>
						<select name="addUsers[]" multiple>
							$nonMembersList
						</select>
					</td>
				</tr>
				<tr>
					<td colspan="2" align="center">
						<input type="hidden" name="activeList" value=$activeList" />
						<input type="submit" name="saveUsers" value="Save" />
					</td>
				</tr>
			</table>
		</form>
HTMLBLOCK;

		$finalHTML .= $listForm.$userControlForm;
	}
}
else{ //default to loading the list creation page
	$createForm = <<<HTMLBLOCK
	<form method="post" action="">
		<div>List Name: <input type="text" name="listName" /></div>
		<div>Valid From: <input type="text" name="fromDate" /></div>
		<div>Valid To: <input type="text" name="toDate" /></div>
		<div>Select users to add to the list:</div>
		<div>
			<select name="addUsers[]" multiple>
				$nonMembersList
			</select>
		</div>
		<input type="submit" name="createList" value="Create" />
	</form>
HTMLBLOCK;

	$finalHTML .= $createForm;
}
die($finalHTML);
?>