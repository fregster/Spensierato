<?php
common::requires_session();
define('CART', 'shopping_cart', true);
define('ITEM', 'item', true);
if(!isset($_SESSION[CART]))
{
	$_SESSION[CART] = array();
}
$_SESSION[CART] = array(
1 => array('title'=> 'Random Title', 'price' => 10, 'shipping' => 5, 'quantity'=>1, 'product_id' => 1),
'cost'=> array('price'=>10, 'shipping'=>5, 'tax'=>1, 'total'=>16),
'updated' => 123456,
);
class shoppingcart
{
	public static function add_to_cart($product_id, $quantity)
	{
		$data = shoppingcart::get_product_data($product_id);
		if(!isset($_SESSION[CART][ITEM][$product_id]))
		{
			$_SESSION[CART][ITEM][$product_id] = array('name' => $data['title'], 'price' => $data['price'], 'shipping' => $data['shipping'], 'product_id' => $product_id);
		}
		return shoppingcart::update_cart_object($product_id, $quantity);
	}
	
	public static function get_cart()
	{
		return common::array_to_xml_simple($_SESSION[CART], 'cart', 'item');
	}
	
	public static function lastmodified()
	{
		return $_SESSION[CART]['updated'];
	}
	
	public static function update_cart_object($product_id, $quantity)
	{
		if(isset($_SESSION[CART][ITEM][$product_id]))
		{
			if($quantity <= 0)
			{
				unset($_SESSION[CART][ITEM][$product_id]);	
				return true;
			}
			$_SESSION[CART][ITEM][$product_id]['quantity'] = $quantity;
			return true;
		}
		return false;
	}
	
	public static function delete_cart_object($id)
	{
		return shoppingcart::update_cart_object($id, 0);
	}
	
	private static function get_product_data($id)
	{
		
	}
	
	private static function calculate_price()
	{
		$price = 0;
		$shipping = 0;
		
		foreach($_SESSION[CART][$product_id] as $product)
		{
			$price = $price + ($product['quantity'] * $product['price']);
			$shipping = max($shipping, $product['shipping']);
		}
		$total = $price+$shipping;
		
		$result = array('price' => $price, 'shipping' => $shipping);
		
		if(settings::singleton()->get_setting('shop_tax_rate'))
		{
			$tax = (settings::singleton()->get_setting('shop_tax_rate')*$price) + (settings::singleton()->get_setting('tax_rate')*$shipping);
			$total = $total + $tax;
			$result['tax'] = $tax;
		}
		$result['total'] = $total;
		$_SESSION[CART]['cost'] = $result;
		return $result;
	}
}
?>