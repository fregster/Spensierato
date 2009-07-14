<?php
common::requires_session();
class shoppingcart
{
	public static function add_to_cart($product_id, $name, $price, $quantity)
	{
		
	}
	
	public static function get_cart()
	{
		
	}
	
	public static function update_cart_object($id, $quantity)
	{
		
	}
	
	public static function delete_cart_object($id)
	{
		return shoppingcart::update_cart_object($id, 0);
	}

?>