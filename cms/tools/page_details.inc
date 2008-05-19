<?php
if(Security::singleton()->can_view(CMS::Singleton()->path_vars[1]))
{
?>
<div>
	<form id="page_details" method="POST" action="">
	<div>
		Page Title <input type="text" value="">
		Hidden <input type="" />
		Delete <input>
		
	</div>
	</form>
</div>
<?php
}
else
{
	error::do403();
}
?>