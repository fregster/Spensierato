<?php
class spellcheck
{
	public function spellcheck_autocorrect($string, $highlight = true)
	{
		if(function_exists('pspell_new'))
		{
			$spell = pspell_new_personal(Settings::singleton()->get_setting('cms_folder_search').DS.'custom_spellings.dic', Settings::singleton()->get_setting('pspell_lang'), Settings::singleton()->get_setting('pspell_spelling'));
			
			$words = split(' ',$string);
			$res = '';
			$res2 = '';
			
			foreach ($words as $value) 
			{
				$check = preg_split('/[\W]+?/',$value);
				if (isset($check[1]) && ($check[1] != '') and (strpos("'",$value) > 0) ) {$check[0] = $value;}
				if (($check[0] + 1 == 1) and (!pspell_check($spell, $check[0]) )) 
				{
					if($highlight)
					{
						$res  .= '<span class="misspelled" style="color:#FF0000; font-weight:bold;">' . $value . ' </span> ';
					}
					else
					{
						$res  .= $value.' ';
					}
				
					$poss = pspell_suggest($spell,$value);
					$orig = metaphone($value);
					foreach ($poss as $suggested)
					{
						$ranked[metaphone($suggested)] = $suggested;
					}
					if($ranked[$orig] <> '')
					{
						$poss[1] = $ranked[$orig];
					}    

					if($highlight)
					{
						$res2  .= '<span style="color:#CC8800; font-weight:bold">' . $poss[1] . ' </span> ';
					}
					else
					{
						$res2  .= $poss[1].' ';
					}
					
				}
				else
				{
					$res .= $value . ' ';
					$res2 .= $value . ' ';
				}
			}
	
			$n[1] = trim($res);
			$n[2] = trim($res2);
			return $n;
		}
		return $string;
	}
	
	private function check_word($word)
	{

	}
}