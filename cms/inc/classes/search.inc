<?php
/**
 * Perform a simple text replace
 * This should be used when the string does not contain HTML
 * (off by default)
 */
define('STR_HIGHLIGHT_SIMPLE', 1);

/**
 * Only match whole words in the string
 * (off by default)
 */
define('STR_HIGHLIGHT_WHOLEWD', 2);

/**
 * Case sensitive matching
 * (off by default)$id
 */
define('STR_HIGHLIGHT_CASESENS', 4);

/**
 * Overwrite links if matched
 * This should be used when the replacement string is a link
 * (off by default)
 */
define('STR_HIGHLIGHT_STRIPLINKS', 8);
		
/**
 * 
 * Simple old school style class for providing a standard interface for a search engine.
 * The class is more a container and interfaced through a single Singleton object.
 * 
 * @author paulfryer
 *
 */
class Search
{
	private static $instance;
	private $lucene_search = false;

	private $tokens = array(); //Array of search words
	private $max_results = 20; //Maximum number of result the method will return
	private $max_results_per_query = 200; //Maximum number of results the db will return per request (Limit to reduce memory at the expense of posibly more db requests)
	private $boolean; //set in search() method
	private $string; //the search string entered

	private $page; //Are we after a specific page of results. set in search() method
	private $drop; //Do we need to drop X results? Used in paging ie $result_per_pag X $page
	private $results_per_page; //set in search() method
	private $return_summary;

	//Vars used in processing
	private	$droped;
	private $results_in_set;

	//Vars set during the construct
	private $time;
	protected $enabled = true;

	//Vars built
	private $ids = array();
	private $results = array();
	private $page_to_elements = array();
	private $element_to_page = array();

	public static function Singleton()
	{
		if(Settings::singleton()->get_setting('search'))
		{
		   if (!isset(self::$instance))
		   {
			   $c = __CLASS__;
			   self::$instance = new $c;
		   }

		   return self::$instance;
		}
		else{
			return NULL; //should we return false here?
		}
	}
	
	public function enabled()
	{
		return $this->disabled;
	}
	
	/**
	 * 
	 * Is Lucence enabled and available?
	 * If it is not available but is enabled attempt to create the index
	 */
	public static function lucence()
	{
		//Can we use lucene search?
		if(Settings::singleton()->get_setting('use_lucence_search'))
		{
			if(is_readable(Settings::singleton()->get_setting('cms_folder_search').DS.'search_index'.DS.'segments.gen'))
			{
	    		return true;
			}
			else if (is_writable(Settings::singleton()->get_setting('cms_folder_search')) && user::Singleton()->is_admin())
			{
				if(!file_exists(Settings::singleton()->get_setting('cms_folder_search').DS.'search_index'))
				{
					@mkdir(Settings::singleton()->get_setting('cms_folder_search').DS.'search_index');
				}
				if(new Zend_Search_Lucene(Settings::singleton()->get_setting('cms_folder_search').DS.'search_index', true))
				{
					Warnings::Singleton()->register('Enhanced Search Created', 'The enhanced search created an index rebuild will be required');
					return true;
				}
			}
			Warnings::Singleton()->register('Enhanced Search Failed', 'The enhanced search was unable to initialise. Check your folder permissions');
		}
		
		return false;
	}

	private function __construct(){
		//these values must be initialized to 0!
		//Do not change these!
		$this->dropped = 0;
		$this->results_in_set = 0;
		
		if(search::lucence())
		{
			$this->lucene_search = new Zend_Search_Lucene(Settings::singleton()->get_setting('cms_folder_search').DS.'search_index');
		}
		else
		{
			//Does the DB support search as Lucene is not available?
			if(!Database::Singleton()->fts()){
				$this->enabled = false;
			}
		}
	}

	public function __clone()
	{
	   trigger_error('Clone is not allowed.', E_USER_ERROR);
	}

	/**
	 * search(string, optional number of results, boolean mode = false)
	 * This is the public method for searching the backend database
	 * $element_types 	NULL = no restrictions
	 * 					1 = not documents
	 * 					2 = elements with files only
	 */
	public function search($string, $element_types = NULL, $results_per_page = 20, $page = 0, $boolean = false, $return_summery = true)
	{
		$this->time = time();
		$this->results_per_page = $results_per_page;
		$this->return_summary = $return_summery;
		$this->page = $page;
		$this->drop = $this->page * $this->results_per_page;
		$this->boolean = $boolean;
		$this->string = strtolower($string);
		$this->corrected = spellcheck::spellcheck_autocorrect($this->string, false);
		$this->tokens = explode(' ', $this->string);
		if(strlen($this->string) > 0)
		{
			$this->get_results();
			if($this->return_summary)
			{
				$this->highlight();
			}
		}
		return($this->build_xml());
	}

	private function highlight()
	{
		if(isset($this->results[0]))
		{
			foreach($this->results as $key => $result){
				$this->results[$key]['html'] = $this->process_search_text(stripslashes($result['search']));
				unset($this->results[$key]['search']);
			}
		}
	}

	private function build_xml()
	{
		$xml = '<?xml version="1.0" encoding="utf-8"?>
<search starting="'.($this->drop +1).'" finishing="'.($this->drop + $this->results_in_set).'" page="'.($this->page+1).'" rpp="'.$this->results_per_page.'">
	<string>'.$this->string.'</string>';
		
		if($this->corrected != null && $this->corrected[2] != $this->string)
		{
			$xml .= '<corrected>'.$this->corrected[2].'</corrected>';
		}
		
//		$xml .= '<number_of_results>'.$this->results_in_set.'</number_of_results>';

		$xml .= xml::array_to_xml_simple($this->results, 'results', 'result');

		$xml .= '<time>'.(time()-$this->time).'</time></search>';

		return $xml;
	}

	/**
	 * 
	 * The main method to get the results and process the results returned from the DB to account for security etc
	 */
	private function get_results()
	{		
		$results = $this->get_results_from_index();
//		$this->build_link_array();
		$security = security::Singleton()->bulk_page_security_check($this->ids);

		foreach($this->ids as $key => $id)
		{
			if(isset($security) && is_numeric($id) && !isset($security[$id]))
			{
				unset($this->ids[$key]); //Drop from the array's list values we do not have security to see
			}
			else
			{
				if($this->droped < $this->drop)
				{
					++$this->droped;
				}
				else
				{
					//We do have permission to view it so push the result into the final results array
					$results[$key]['score'] = round($results[$key]['score'], 1); //Tidy up the result score
					$results[$key]['position'] = $key + $this->drop + 1;
					$this->results[] = $results[$key];
					++$this->results_in_set;
					if($this->results_in_set >= $this->results_per_page)
					{
						break; //We have all we need so don't bother doing any more processing
					}
				}
			}
		}

		if($this->droped < $this->drop) //Do we have all the result we need? if not get more from the DB
		{
			$this->get_results();
		}
	}
	
	/**
	 * 
	 * Standard interface method to always retrieve the search results from the best posible search type
	 */
	private function get_results_from_index()
	{		
		if($this->lucene_search !== false)
		{
			return $this->get_results_from_index_lucene();
		}
		
		//Fall back to simple sql search
		return $this->get_results_from_index_sql();
	}
	
	private function get_results_from_index_sql()
	{
		$pull = array('id', 'title', 'file');
		if($this->return_summary) { $pull[] = 'search'; }
		$results = Database::singleton()->sql_search('tbl_search', 'search', $this->string, $pull, $this->boolean, null, null, $this->drop, $this->max_results_per_query);

		$ids = common::retrieve_values_from_array($results, 'id');
		if($ids)
		{
			$this->ids[] = $ids;
		}
		
		if(isset($results[0]))
		{
			foreach ($results as $key => $result){
				if($results[$key]['file'] == '0'){
					$results[$key]['page'] = $result['id'];
				}
			}
		}
		return $results;
	}
	
	private function get_results_from_index_lucene()
	{
		$results = array();
		
		foreach($this->lucene_search->find($this->string) as $r) //Loop through the results to format them in the required array style
		{
			$page = $r->element_id;
			$result = array('id' => $r->element_id, 'score' => $r->score, 'title' => $r->title);
			if(@$r->file) //If this result is a file
			{
				$result = database::singleton()->sql_select_first(array('page_id'), 'tbl_page_elements', array('element_id'), array($r->element_id), array('main'));
				$page = $result['page_id'];
				$result['file'] = true;
			}
			$this->ids[] = $page;
			$result['page'] = $page;
			if($this->return_summary) { $result['search'] = $r->search;}
			$results[] = $result;
			unset($result);
		}
		return $results;
	}

	private function build_link_array()
	{
		$pages = database::singleton()->sql_select(array('page_id', 'element_id'), 'tbl_page_elements', array('element_id', 'visible'), array($this->ids, 1), array('main'));
		if ($pages != NULL) {
			foreach($pages as $page)
			{
				if(!empty($page['element_id']) && !empty($page['page_id']))
				{
					//Array ( [page_id] => 1 [element_id] => 1 ) )
					if(!isset($this->page_to_elements[$page['page_id']]))
					{
						$this->page_to_elements[$page['page_id']] = array();
					}
					$this->page_to_elements[$page['page_id']][] = $page['element_id'];

					if(!isset($this->page_to_elements[$page['element_id']]))
					{
						$this->page_to_elements[$page['element_id']] = array();
					}
					$this->element_to_page[$page['element_id']][] = $page['page_id'];
				}
			}
		}
	}

	/**
	 * process_search_text($result)
	 * 
	 * This processes each results search text from the database
	 * It goes through the text looking for each key word and builds an array of there positions
	 * It then goes through the text to each term, goes back X chars and then to the previous space (To get the complete word)
	 * It then goes forwards from the term X letters, if it comes accross another term it goes X letters from that term until the string is complete
	 * The string is then stored and the number of terms in that string is also sotred so that the most term rich strings can be displayed first
	 * The final string is built by concaternation the relevent strings and escaped if it goes over the escape output length and is hard cut off at the max output length
	 * 
	 * @param $result
	 * @return string
	 */
	private function process_search_text($result)
	{
		$number_of_letters = 40;
		$escape_output_length = 300;
		$max_output_length = 400;
		$output_text = '...';
		$output_array_store = array();
		$output_array_keys = array();
		$i = 0;

		//word_possition[position in string] => length
		$word_position = array();
		$max_length = strlen($result);

		if($max_length >= $number_of_letters)
		{
			foreach($this->tokens as $term) //Run through the term and find the position of each term in the string
			{
				$termlen = strlen($term);
				$pos=-1;
				$pos_array = array();
				while (($pos=stripos($result,$term,$pos+1))!==false) $pos_array[$i++]=$pos;

				foreach($pos_array as $key => $p)
				{
					$word_position[$p] = $termlen;
				}
			}

			ksort($word_position);
			$end_position = 0;


			foreach($word_position as $pos => $len) //For each word in the string
			{
				//Number of key words used in this sentance
				$key_words = 1;

				//Find the start
				$start_position = $pos - $number_of_letters;
				if($start_position < 0) { $start_position = 0; }

				$current_char = '';
				while($current_char != ' ' && $start_position > 0) //Keep steping back from the term in the string untill we get to a space (Complete word)
				{
					$current_char = $result[$start_position--];
				}
				$start_position += 2;

				$current_char = '';
				$temp_position = $start_position;
				while($current_char != '. ' && $temp_position < $pos) //Step forwards untill we get to the end of a sentance
				{
					$current_char = $result[$temp_position].$result[++$temp_position];
				}
				if($current_char == '. ') { $start_position = $temp_position; }
				if($start_position < $end_position) //Check it does not turn out that were actually extending the previous string
				{
					//Trim the last three . from the output text as this is part of the same sentance
					//TODO This might need some thinking about now we re-order the output.
					$output_text = substr($output_text, 0, -5);
					$start_position = $end_position;
				}

				//Find the end
				$end_position = $pos + $len + $number_of_letters;
				while(key($word_position) < $end_position)
				{
					$end_position = key($word_position) + current($word_position) + $number_of_letters;
					++$key_words;
					if(next($word_position) === false) { break; }
				}

				$current_char = '';
				$i = 0;
				while($current_char != ' ' && $end_position < $max_length && $i < 100)
				{
					$current_char = $result[$end_position++];
					++$i;
				}

				$current_char = '';
				$temp_position = $end_position;
				while($current_char != '. ' && $temp_position > ($pos + $len) )
				{
					$current_char = $result[($temp_position -1)].$result[$temp_position--];
				}

				if($current_char == '. ') { $end_position = $temp_position; }

				$output_text = substr($result, $start_position, ($end_position - $start_position));
				$output_length = strlen($output_text);

				//Store the array for relevance sorting
				$output_array_store[] = array('length'=> $output_length, 'text'=>$output_text);
				$output_array_keys[] = $key_words;

			}

			//Sort the output keys into order with the text with the most keys (Most likley to be relevent) first
			arsort($output_array_keys);

			$output_length = 0;
			$output_text = '...';

			foreach($output_array_keys as $key => $number_of_keys)
			{
				$output_text .= $output_array_store[$key]['text'] .' ... ';
				$output_length = $output_length + $output_array_store[$key]['length'];

				if($output_length > $escape_output_length) {
					if($output_length > $max_output_length) { $output_text = substr($output_text, 0, $max_output_length).' ...'; }
					break;
				}
			}
			return $this->process_search_text_simple($output_text);
		}
		return $this->process_search_text_simple($result);
	}

	private function process_search_text_simple($result)
	{
		$result = strip_tags($result);

		foreach($this->tokens as $term)
		{
			if(strlen($term) > 0)
			{
				$result = $this->str_highlight($result, $term);
			}
		}
		return $result;
	}

	private function str_highlight($text, $needle, $options = null, $highlight = null)
	{
		/**
		 * Highlight a string in text without corrupting HTML tags
		 *
		 * @author      Aidan Lister <aidan@php.net>
		 * @version     3.1.1
		 * @link        http://aidanlister.com/repos/v/function.str_highlight.php
		 * @param       string          $text           Haystack - The text to search
		 * @param       array|string    $needle         Needle - The string to highlight
		 * @param       bool            $options        Bitwise set of options
		 * @param       array           $highlight      Replacement string
		 * @return      Text with needle highlighted
		 */

	    // Default highlighting
	    if ($highlight === null) {
	       // $highlight = '&lt;span&gt;\1&lt;/span&gt;';
	       $highlight = '&lt;strong&gt;\1&lt;/strong&gt;';
	    }

	    // Select pattern to use
	    if ($options & STR_HIGHLIGHT_SIMPLE) {
	        $pattern = '#(%s)#';
	        $sl_pattern = '#(%s)#';
	    } else {
	        $pattern = '#(?!<.*?)(%s)(?![^<>]*?>)#';
	        $sl_pattern = '#<a\s(?:.*?)>(%s)</a>#';
	    }

	    // Case sensitivity
	    if (!($options & STR_HIGHLIGHT_CASESENS)) {
	        $pattern .= 'i';
	        $sl_pattern .= 'i';
	    }

		$needle = (array) $needle;
		foreach ($needle as $needle_s) {
	        $needle_s = preg_quote($needle_s);

	        // Escape needle with optional whole word check
	        if ($options & STR_HIGHLIGHT_WHOLEWD) {
	            $needle_s = '\b' . $needle_s . '\b';
	        }

	        // Strip links
	        if ($options & STR_HIGHLIGHT_STRIPLINKS) {
	            $sl_regex = sprintf($sl_pattern, $needle_s);
	            $text = preg_replace($sl_regex, '\1', $text);
	        }

	        $regex = sprintf($pattern, $needle_s);
			$text = preg_replace($regex, $highlight, $text);
		}

	    return $text;
	}
	
	/**
	 * 
	 * Public method to recreate the search index object for a specific element, this forces a complete page build that this element exsists on
	 * @param element ID int $id
	 */
	public function add_to_index($id)
	{
		$element = Element::type($id); //Quickly create a blank element of the same type to see if it has files
		if($element->allows_attachments()) {
			$this->build_file_index($id);
		}
		unset($element);
		
		//Check the element type, if it a file element it should be indexed seperatly and the title and keywords added to the page index
		
		//Get the pages that this element is on and the list of element ids
		$pages = Database::Singleton()->sql_select(array('page_id'), 'tbl_page_elements', array('element_id'), array($id));
		foreach($pages as $page)
		{
			$this->build_page_index($page['page_id']);
		}
	}
	
	private function build_file_index($element_id)
	{
		$fields = array('id', 'title', 'indexed', 'file', 'search');
		$element = Element::type($id);
		
		$values = array($element_id, $element->title(), 0, 1, $this->get_search_string_for_elements(array($element_id)));
		$this->index($fields, $values);
	}
	
	//array('id', 'title', 'indexed', 'file', 'search');
	private function index($fields, $values)
	{
		/**
		 * If were using the lucence search index update it now
		 */
		$values[1] = strtolower($values[1]);
		$values[2] = $this->add_to_lucene($values[0], $values[3], $values[1], $values[4]);
		
		/**
		 * Update the SQL search text
		 */
		database::singleton()->sql_update_or_insert('tbl_search', array('id', 'file',), array($values[0], $values[3]), $fields, $values, true, 'search_id');
	}
	
	public function build_page_index($page_id)
	{
		$fields = array('id', 'title', 'indexed', 'file', 'search');
		$title = Database::Singleton()->sql_select_first(array('title'), 'tbl_pages', array('id'), array($page_id));
		$values = array($page_id, $title['title'], 0, 0, NULL);
		unset($title);
		
		$values[4] = $this->get_search_string_for_elements(common::retrieve_values_from_array(Database::Singleton()->sql_select(array('element_id'), 'tbl_page_elements', array('page_id', 'visible'), array($page_id, 1)), 'element_id'));
		
		$this->index($fields, $values);
	}
	
	private function get_search_string_for_elements($ids, $expand_files = false)
	{
		$text = '';
		
		//For each element, build the element object to then get the search text and build up the page search string
		$data = Database::Singleton()->sql_select(array('*'), 'tbl_elements', array('id'), array($ids), array('id'));
		$element_data = Database::Singleton()->sql_select(array('*'), 'tbl_elements_data', array('id'), array($ids), array('id'));

		if(isset($data[0]))
		{
			foreach($data as $key => $data)
			{
				if(isset($element_data[$key]) && $element_data[$key] != NULL)
				{
					$element = Element::type($data['type']);
					$element->build($element_data[$key]['id'], $element_data[$key]);
					$text .= ' '.$element->get_search_text($expand_files);
				}
			}
		}
		return $text;
	}
	
	private function add_to_lucene($id, $file, $title, $text, $keywords = false)
	{
		if($this->lucene_search !== false)
		{
			$doc = new Zend_Search_Lucene_Document();
			$doc->addField(Zend_Search_Lucene_Field::Keyword('element_id', $id));
			$doc->addField(Zend_Search_Lucene_Field::Text('file', $file));
			$doc->addField(Zend_Search_Lucene_Field::Text('title', $title));
			$doc->addField(Zend_Search_Lucene_Field::Text('search', $text));
		
			if($keywords !== false)
			{
				$doc->addField(Zend_Search_Lucene_Field::Text('keywords', $keywords));
				$fields[] = 'keywords';
				$values[] = $keywords;
			}
			
			//Ensure we delete the old record
			$this->drop_from_lucene($id, $file);
			$this->lucene_search->addDocument($doc);
			
			return 1;
		}
		return 0;
	}
	
	public function drop_from_index($id, $file)
	{
		$this->drop_from_sql($id, $file);
		$this->drop_from_lucene($id, $file);
	}
	
	private function drop_from_sql($id, $file)
	{
		
	}
	
	private function drop_from_lucene($id, $file)
	{
		if($this->lucene_search !== false)
		{
			$keyword = 'page_id';
			$element = Element::type($id); //Quickly create a blank element of the same type to see if it has files
			if($element->allows_attachments()) {
				$keyword = 'file';
			}
			unset($element);
			
			$l_id = $this->lucene_search->find($keyword.':' . $id);
			if(isset($l_id->id) && is_numeric($l_id->id)) { 
				$this->lucene_search->delete($l_id->id);
			}
		}
	}

}