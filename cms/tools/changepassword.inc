<?php

//define all the error codes
define('PASS_EMPTY_OLD', 1);
define('PASS_EMPTY_NEW', 2);
define('PASS_WRONG_OLD', 3);
define('PASS_IDENTICAL', 4);
define('PASS_SUCCESS', 5);

//function to reload the page with the error code
function send_msg($error){
	common::redirect('tools/changepassword/'.$error);
}

//set the html for the password form
$passwordForm = <<< HTMLBLOCK
<div id="password_box">
	<form method="post" action="" id="password">
		<div>Old Password: <input type="password" id="oldPass" name="oldPass" /></div>
		<div>New Password: <input type="password" id="newPass" name="newPass" /></div>
		<div><input type="submit" name="savePass" value="Change Password" /></div>
	</form>
</div>
HTMLBLOCK;

//set the html for successful password save
$successHTML = <<<HTMLBLOCK
<body onload='setTimeout("window.top.hidePopWin(false);",3000);'>
	<div>Your password has been changed successfully!</div>
	<div><button type="button" onclick="window.top.hidePopWin(false);">Click to close</button></div>
</body>
HTMLBLOCK;

$postVars = new post;
$posted = $postVars->variable('savePass');

//if the form was posted
if (!$posted) {

	//if an error code was set
	if (isset(CMS::Singleton()->path_vars[2])) {

		//start the error code msg text
		$error = '<div>ERROR: ';

		//check which error occured and create msg accordingly
		switch(CMS::Singleton()->path_vars[2]){
			case 1:
				$error .= 'You forgot to enter your old password!';;
				break;
			case 2:
				$error .= 'You forgot to enter your new password!';
				break;
			case 3:
				$error .= 'The old password you entered is wrong!';
				break;
			case 4:
				$error .= 'The new password you entered is identical to the old one!';
				break;
			case 5:
				$passwordForm = $successHTML;
				unset($error);
				break;
		}
		$error .= '</div>';
	}

	//display form and error (if exists)
	die($error.$passwordForm);

} //end if

//check for errors
if (!$postVars->variable('oldPass')) send_msg(PASS_EMPTY_OLD);
if (!$postVars->variable('newPass')) send_msg(PASS_EMPTY_NEW);

//get the user id and hash the old/new password values
$userid = user::Singleton()->id();
$hashedNewPass = common::return_hash($postVars->variable('newPass'), false);
$hashedOldPass = common::return_hash($postVars->variable('oldPass'), false);

//verify that the old password given is the correct one.
$verifyInfo = database::Singleton()->sql_select_first(array('id'), 'tbl_users', array('id', 'password'), array($userid, $hashedOldPass));

//check for incorrect password
if (is_null($verifyInfo)) send_msg(PASS_WRONG_OLD);

//check for indentical passwords
if (!strcmp($postVars->variable('oldPass'), $postVars->variable('newPass'))) send_msg(PASS_IDENTICAL);

//if no errors occured, save the new password
database::Singleton()->sql_update_by_id($userid, 'tbl_users', array('password'), array($hashedNewPass));

//display the success msg
send_msg(PASS_SUCCESS);
?>