<?php
class setup
{
	private static $instance;
	private $store = array();
	private $class_construct_store = array();

	public static function Singleton()

	{
	   if (!isset(self::$instance)) {
		   $c = __CLASS__;
		   self::$instance = new $c;
	   }

	   return self::$instance;
	}

	public function __clone()
	{
	   trigger_error('Clone is not allowed.', E_USER_ERROR);
	}
	
	public function save($name, $values){
		$this->store[$name] = $values;
	}
	
	public function load($name){
		if(isset($this->store[$name])){
			return $this->store[$name];
		}
		return false;
	}
	
	public function save_construct($class, $method, $params){
		$this->class_construct_store[$class] = array('method' => $method, 'params' => $params);
	}
	
	public function load_construct($class)
	{
		if(isset($this->class_construct_store[$class])){
			$value = $this->class_construct_store[$class];
			unset($this->class_construct_store[$class]); //As this is for the auto load we no longer need this so delete it
			return $value;
		}
		return false;
	}
}