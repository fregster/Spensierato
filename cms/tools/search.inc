You search for <em><? echo($_GET['s']); ?></em>
<div>Search Results</div>

<br /><br />
Modify my search
<form action="" onsubmit="javascript:searchFunction(); return false;" id="searchform">
                <input type="text" id="searchtext" name="searchtext" value="<? echo($_GET['s']); ?>" />
                <input type="button" name="button" value="Find" onclick="javascript:searchFunction();" />
</form>