<?php
//page security
if (!user::Singleton()->is_logged_in() || !Security::singleton()->can_approve()) common::bounce_to_referer();


#############
# functions #
#############

function find_item_by_id($id, $id_col, $array){
	foreach ($array as $key => $item){
		if ($item[$id_col] == $id)
			return $key;
	}
	return false;
}

function check_dates($date1, $date2){ //temporary date checking function to get the tool working
	$ts1 = strtotime($date1);
	$ts2 = strtotime($date2);

	if ($ts1 < $ts2 || !$ts2) {
		return true;
	}

	return false;
}


##########################
# data init for the tool #
##########################

$lists = database::Singleton()->sql_select(array('id', 'name', 'valid_from', 'valid_to'), 'tbl_list_data');

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

			if (check_dates($postVars->variable('fromDate'), $postVars->variable('toDate'))) {

				CMS::Singleton()->path_vars[2] = 'edit';
				$insertFields = array('name', 'valid_from', 'valid_to', 'created', 'created_by');

				$insertValues = array($postVars->variable('listName'));
				$insertValues[] = ($postVars->variable('fromDate')?strtotime($postVars->variable('fromDate')):time());
				$insertValues[] = ($postVars->variable('toDate')?strtotime($postVars->variable('toDate')):'0');
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
			else $finalHTML .= '<div>Error: The "Valid To" date is earlier than the "Valid From" date.</div>';
		}
		else $finalHTML .= '<div>Error: A list with that name already exists.</div>';
	}
	else $finalHTML .= '<div>Error: You forgot to provide a list name.</div>';
}
else if($postVars->variable('saveUsers')){
	$activeList = $postVars->variable('activeList');

	if ($postVars->variable('removeUsers'))
		database::Singleton()->sql_delete_where('tbl_list_membership', array('user_id'), array($postVars->variable('removeUsers')));

	if ($postVars->variable('addUsers')) {
		foreach ($postVars->variable('addUsers') as $user){
			database::Singleton()->sql_insert('tbl_list_membership', array('list_id', 'user_id'), array($activeList, $user));
		}
		unset($user);
	}

	if ($postVars->variable('validFrom')) $validFromTS = strtotime($postVars->variable('validFrom'));
	else $validFromTS = time();

	if ($postVars->variable('validTo')) $validToTS = strtotime($postVars->variable('validTo'));
	else $validToTS = 0;

	database::Singleton()->sql_update_by_id($activeList, 'tbl_list_data', array('valid_from', 'valid_to'), array($validFromTS, $validToTS));

	//re-fetch data after the save
	//TODO - change the page to use xml so this update is irrelevant
	$lists = database::Singleton()->sql_select(array('id', 'name', 'valid_from', 'valid_to'), 'tbl_list_data');
	$members = database::Singleton()->sql_select(array('user_id'), 'tbl_list_membership', array('list_id'), array($activeList));
}
else if ($postVars->variable('changeList')) {
	$activeList = $postVars->variable('chooseList');
	$members = database::Singleton()->sql_select(array('user_id'), 'tbl_list_membership', array('list_id'), array($activeList));
}

#######################
# construct html form #
#######################

if ($lists) {
	if ($toDate = $lists[find_item_by_id($activeList, 'id', $lists)]['valid_to']) {
		$toDate = date('d/m/Y', $lists[find_item_by_id($activeList, 'id', $lists)]['valid_to']);
	}
	$validDates = array('from' => date('d/m/Y', $lists[find_item_by_id($activeList, 'id', $lists)]['valid_from']),
						'to' => ($toDate?$toDate:''));
}

$nonMembersList = '';
if ($users) {
	$nonMembersList = array();
	foreach ($users as $user){
		if ($members && (find_item_by_id($user['id'], 'user_id', $members) === false) || (CMS::Singleton()->path_vars(2) != 'edit'))
			$nonMembersList[] = '<option value="'.$user['id'].'">'.$user['username'].'</option>';
	}
	unset($user);
	$nonMembersList = implode('', $nonMembersList);
}


if (CMS::Singleton()->path_vars(2) == 'edit') {
	if ($lists) {

		$listList = array();
		$membersList = array();

		foreach ($lists as $list){
			$listList[] = '<option value="'.$list['id'].'">'.$list['name'].'</option>';
		}
		unset($list);
		$listList = implode('', $listList);

		foreach ($members as $member){
			$membersList[] = '<option value="'.$member['user_id'].'">'.$users[find_item_by_id($member['user_id'], 'id', $users)]['username'].'</option>';
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
					<td>Valid from: <input type="text" name="validFrom" value="{$validDates['from']}" /></td>
					<td>Valid to: <input type="text" name="validTo" value="{$validDates['to']}" /></td>
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
	else $finalHTML .= '<p>There are no lists to edit.</p>';
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