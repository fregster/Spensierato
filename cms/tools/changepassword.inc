<?php
if (!user::Singleton()->is_logged_in()) common::bounce_to_referer();

//define all the error codes
define('PASS_EMPTY_OLD', 1);
define('PASS_EMPTY_NEW', 2);
define('PASS_WRONG_OLD', 3);
define('PASS_IDENTICAL', 4);
define('PASS_CONFIRM', 5);
define('PASS_SUCCESS', 6);

//function to reload the page with the error code
function send_msg($error){
	common::redirect('tools/changepassword/'.$error);
}

//set the html for the password form
$passwordForm = <<< HTMLBLOCK
<div id="password_box">
	<form method="post" action="" id="password">
		<table>
			<tr>
				<td>Old Password:</td>
				<td><input type="password" id="oldPass" name="oldPass" /></td>
			</tr>
			<tr>
				<td>New Password:</td>
				<td><input type="password" id="newPass" name="newPass" /></td>
			</tr>
			<tr>
				<td>Confirm New Password:</td>
				<td><input type="password" id="newPassConf" name="newPassConf" /></td>
			<tr>
				<td colspan="2">
					<input type="checkbox" id="emailPass" name="emailPass" />
					<label for="emailPass"> Email me my new password.</label>
				</td>
			</tr>
			<tr>
				<td colspan="2"><input type="submit" name="savePass" value="Change Password" /></td>
			</tr>
		</table>
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
	$error = CMS::Singleton()->path_vars(2);
	if (isset($error)) {

		//start the error code msg text
		$error = '<div>ERROR: ';

		//check which error occured and create msg accordingly
		switch(CMS::Singleton()->path_vars(2)){
			case 1:
				$error .= 'You forgot to enter your old password!</div>';;
				break;
			case 2:
				$error .= 'You forgot to enter your new password!</div>';
				break;
			case 3:
				$error .= 'The old password you entered is wrong!</div>';
				break;
			case 4:
				$error .= 'The new password you entered is identical to the old one!</div>';
				break;
			case 5:
				$error .= 'The password confirmation is wrong!</div>';
				break;
			case 6:
				$passwordForm = $successHTML;
				$error = '';
				break;
		}
	}

	//display form and error (if exists)
	die($error.$passwordForm);

} //end if

//check for errors
if (!$postVars->variable('oldPass', 'string')) send_msg(PASS_EMPTY_OLD);
if (!$postVars->variable('newPass', 'string')) send_msg(PASS_EMPTY_NEW);
if (strcmp($postVars->variable('newPass', 'string'), $postVars->variable('newPassConf', 'string'))) send_msg(PASS_CONFIRM);

//get the user id and hash the old/new password values
$userid = user::Singleton()->id();
$hashedNewPass = common::return_hash($postVars->variable('newPass', 'string') . Settings::singleton()->get_setting('random_key'), false);
$hashedOldPass = common::return_hash($postVars->variable('oldPass', 'string') . Settings::singleton()->get_setting('random_key'), false);

//verify that the old password given is the correct one.
$verifyInfo = database::Singleton()->sql_select_first(array('id'), 'tbl_users', array('id', 'password'), array($userid, $hashedOldPass));

//check for incorrect password
if (is_null($verifyInfo)) send_msg(PASS_WRONG_OLD);

//check for indentical passwords
if (!strcmp($postVars->variable('oldPass', 'string'), $postVars->variable('newPass', 'string'))) send_msg(PASS_IDENTICAL);

//if no errors occured, save the new password
database::Singleton()->sql_update_by_id($userid, 'tbl_users', array('password'), array($hashedNewPass));

//email the password to the user if requested
if ($postVars->variable('emailPass')) {
	$to = user::Singleton()->email();
	$from = Settings::singleton()->get_setting('site_name_long').' <no-reply@'.str_replace('www.', '', $_SERVER['SERVER_NAME']).'>';
	$subject = 'New password at '.settings::singleton()->get_setting('site_name_long');
	$message = '<p>Dear user. You have changed your password.<br /><br />The new password is: '.$postVars->variable('newPass', 'string').'.
				<br />Please keep your passwords safe and secret.<br />
				<br />Best regards, <br />'.Settings::singleton()->get_setting('site_name_long').' team.</p>';

	common::email($to, $from, $subject, $message, true);
}

//display the success msg
send_msg(PASS_SUCCESS);