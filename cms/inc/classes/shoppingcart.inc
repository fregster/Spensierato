<?php
define('CART', 'shopping_cart', true);
define('ITEM', 'items', true);
define('VERSION', 'version', true);

/*
 * Example cart array
 * the items array is keyed with the product id for quick updating, this numeric value is turned into 'item' in the xml conversion
 */
//$_SESSION[CART] = array(
//'items' => array(1 => array('title'=> 'Random Title', 'price' => 10, 'shipping' => 5, 'quantity'=>1, 'product_id' => 1)),
//'cost'=> array('price'=>10, 'shipping'=>5, 'tax'=>1, 'total'=>16),
//'updated' => 123456,
//);

class shoppingcart
{
	public static function add_to_cart($product_id, $quantity)
	{
		common::requires_session();
		if(!isset($_SESSION[CART]))
		{
			$_SESSION[CART] = array();
			$_SESSION[CART][VERSION] = 1;
		}
		$data = shoppingcart::get_product_data($product_id);
		if($data != false)
		{
			if(!isset($_SESSION[CART][ITEM][$product_id]))
			{
				$_SESSION[CART][ITEM][$product_id] = array('name' => $data['title'], 'price' => $data['Price'], 'shipping' => $data['Shipping'], 'product_id' => $product_id);
			}
			//TODO add tax data
			shoppingcart::update_cart_object($product_id, $quantity);
		}
		return shoppingcart::calculate_price();
	}
	
	public static function get_number_of_items()
	{
		if(isset($_SESSION[CART][ITEM]))
		{
			$count = count($_SESSION[CART][ITEM]);
			if($count > 0)
			{
				return $count;
			}
		}
		return false;
	}
	
	public static function get_cart()
	{
		return common::array_to_xml_simple($_SESSION[CART], 'cart', 'item');
	}
	
	public static function get_version()
	{
		return $_SESSION[CART][VERSION];
	}
	
	public static function lastmodified()
	{
		return $_SESSION[CART]['updated'];
	}
	
	public static function update_cart_object($product_id, $quantity)
	{
		++$_SESSION[CART][VERSION];
		if(isset($_SESSION[CART][ITEM][$product_id]))
		{
			if($quantity <= 0)
			{
				unset($_SESSION[CART][ITEM][$product_id]);
				shoppingcart::calculate_price();
				return true;
			}
			$_SESSION[CART][ITEM][$product_id]['quantity'] = $quantity;
			shoppingcart::calculate_price();
			return true;
		}
		shoppingcart::calculate_price();
		return false;
	}
	
	public static function delete_cart_object($id)
	{
		shoppingcart::update_cart_object($id, 0);
		return shoppingcart::calculate_price();
	}
	
	private static function get_product_data($id)
	{
		if(security::singleton()->can_view_element($id))
		{
			$element = Element::type($id);
			$element->build($id);
			return $element->cart_data();
		}
		return false;
	}
	
	private static function calculate_price()
	{
		$price = 0;
		$shipping = 0;
		
		if(count($_SESSION[CART][ITEM]) > 0)
		{
			foreach($_SESSION[CART][ITEM] as $product)
			{
				$price = $price + ($product['quantity'] * $product['price']);
				$shipping = max($shipping, $product['shipping']);
			}
			$total = $price+$shipping;
			
			$result = array('price' => $price, 'shipping' => $shipping, 'tax' => 0);
			
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
		unset($_SESSION[CART]);
	}
}
?>