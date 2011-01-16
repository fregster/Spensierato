<?php
class xml
{
	public static function xml_merge($xml1, $xml2)
	{
		return array_to_xml(xml::xml_merge_to_array($xml1, $xml2));
	}

	public static function xml_merge_to_array($xml1, $xml2)
	{
		return array_merge_recursive(xml_to_array($xml1), xml_to_array($xml2));
	}

	public static function xml_to_array($xml)
	{
		if(class_exists('persistantcache', false))
		{
			$result = persistantcache::Singleton()->get('xml_to_array_'.$xml);
			if($result != false) { return $result; }
		}

		$xml = simplexml_load_string($xml);

		if(!function_exists('process_children'))
		{
			function process_children($xml)
			{
				$php_array = array();

				foreach ($xml->children() as $child)
				{
					if($child->children())
					{
						$php_array[$child->getName()] = process_children($child);
					}
					else
					{
						$php_array[$child->getName()] = strip_tags($child->saveXML());
					}
				}

				if(class_exists('persistantcache', false))
				{
					persistantcache::Singleton()->save('xml_to_array_'.$xml, $php_array);
				}

				return $php_array;
			}
		}

		return process_children($xml);
	}

	public static function array_to_xml($array, $serializer_options = NULL)
	{
		// An array of serializer options
		if(is_null($serializer_options))
		{
			$serializer_options = array (
			   'addDecl' => FALSE,
			   'encoding' => 'ISO-8859-1',
			   'indent' => '  ',
			   'rootName'  => 'xml',
			   'defaultTagName'  => 'item'
			);
		}
		
		// include class file
		if(!@include_once('XML/Serializer.php'))
		{
			return xml::array_to_xml_simple($array, $serializer_options['rootName'], $serializer_options['defaultTagName']);
		}

		// create object
		$serializer = new XML_Serializer($serializer_options);

		// perform serialization
		$result = $serializer->serialize($array);

		// check result code and display XML if success
		if($result === true)
		{
			return $serializer->getSerializedData();
		}
	}

	public static function array_to_xml_simple($array, $root, $defaulttag = NULL)
	{
		return 	"<$root>".xml::array_to_xml_simple_recursion($array, $defaulttag)."</$root>";
	}

	public static function array_to_xml_simple_recursion($array, $defaulttag = NULL)
	{
		if(is_array($array))
		{
			$xml = '';
			foreach($array as $tag => $value)
			{
				if(is_numeric($tag) and $defaulttag != NULL)
				{
					$tag = $defaulttag;
				}
				if(is_array($value))
				{
					$xml .= "<$tag>".xml::array_to_xml_simple_recursion($value, $defaulttag)."</$tag>";
				}
				else
				{
					$xml .= "<$tag>$value</$tag>";
				}
			}
			return $xml;
		}
	}
	
	public static function node_from_xml($xml, $node)
	{
		$xml_obj = simplexml_load_string($xml);
		if($xml_obj)
		{
			$result= $xml_obj->xpath($node);

			foreach ((array)$result as $result)
			{
		    	return($result);
			}
		}
	}
}