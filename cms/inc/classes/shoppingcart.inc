<?php
define('CART', 'shopping_cart', true);
define('ORDER', 'order', true);
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
				
				if(isset($data['tax']))
				{
					$_SESSION[CART][ITEM][$product_id]['tax'] = round($data['Price'] * $data['tax'], 2);
				}
				else
				{
					$_SESSION[CART][ITEM][$product_id]['tax'] = round($data['Price'] * settings::singleton()->get_setting('shop_tax_rate'), 2);
				}
			}
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
		if(isset($_SESSION[CART]))
		{
			return xml::array_to_xml_simple($_SESSION[CART], 'cart', 'item');
		}
		return false;
	}
	
	public static function get_order()
	{
		if(isset($_SESSION[ORDER]['id']))
		{
			$result = database::singleton()->sql_select_first(array('data'), 'tbl_orders', array('id'), array($_SESSION[ORDER]['id']));
			return xml::array_to_xml_simple(array_merge(json_decode($result['data'],true),array('order' => $_SESSION[ORDER]['id'])), 'order', 'item');
		}
		return false;
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
		$tax = 0;
		$shipping = 0;
		
		if(count($_SESSION[CART][ITEM]) > 0)
		{
			foreach($_SESSION[CART][ITEM] as $product)
			{
				$price = $price + ($product['quantity'] * $product['price']);
				$tax = $tax + ($product['quantity'] * $product['tax']);
				$shipping = max($shipping, $product['shipping']);
			}
			
			//Add the tax from the shipping on
			
			if(settings::singleton()->get_setting('shop_tax_rate'))
			{
				$tax = round($tax + (settings::singleton()->get_setting('shop_tax_rate')*$shipping), 2);
			}
			$result = array('price' => $price, 'shipping' => $shipping, 'tax' => $tax);
			
			$result['total'] = $total = $price+$shipping+$tax;
			$_SESSION[CART]['cost'] = $result;
			return $result;
		}
		unset($_SESSION[CART]);
	}
	
	public static function finalise($email, $details)
	{
		if(count($_SESSION[CART][ITEM]) > 0)
		{
			database::singleton()->force_write_user();
			$_SESSION[ORDER]['id'] = database::singleton()->sql_insert('tbl_orders', array('email', 'data'), array($email, json_encode(array_merge($_SESSION[CART], $details))));
			database::singleton()->destroy_write_user();
			if(is_numeric($_SESSION[ORDER]['id']))
			{
				$_SESSION[ORDER][$_SESSION[ORDER]['id']] = $_SESSION[CART];
				$_SESSION[ORDER][$_SESSION[ORDER]['id']]['details'] = $details;
				shoppingcart::email_confirmation($_SESSION[ORDER]['id'], $email);
				unset($_SESSION[CART]);
				return true;
			}
		}
		return false;
	}
	
	private static function email_confirmation($id, $email)
	{
		if(isset($_SESSION[ORDER][$id]))
		{
			//Build email
			$content = '<p>Thank you for your order at '. settings::singleton()->get_setting('site_name_long') .'. <br/>
Your order will be processed as soon as payment clears and shipped as soon as posible.<br/>
This is NOT A RECIPT OF PAYMENT<br/></p>
<p>';
//			print_r($_SESSION[ORDER][$id]['items']); print_r($_SESSION[ORDER]);
			foreach($_SESSION[ORDER][$id]['items'] as $item)
			{
				$content .= $item['quantity'] . ' X '. $item['name'].' (Product ID #'.$item['product_id'].') individual item value '.settings::singleton()->get_setting('currency_symbol').$item['price'].'<br/>';
			}
			$content .= '<br/>Total including shipping and taxs ' .settings::singleton()->get_setting('currency_symbol').$_SESSION[ORDER][$id]['cost']['total'];
			$content .= '<p>Shipping Address</p><p>'.$_SESSION[ORDER][$id]['details']['first_name'].'<br/>'.$_SESSION[ORDER][$id]['details']['last_name'].'<br/>'.$_SESSION[ORDER][$id]['details']['address1'].'<br/>'.$_SESSION[ORDER][$id]['details']['address2'].'<br/>'.$_SESSION[ORDER][$id]['details']['city'].'<br/>'.$_SESSION[ORDER][$id]['details']['county'];
			$content .= '<p>Contact Details</p><p>'.$_SESSION[ORDER][$id]['details']['email'].'<br/>'.$_SESSION[ORDER][$id]['details']['phone'];		
			$content .='<p>Thank you for your custom</p><p>If you have any questions then please email <a href="mailto:'.settings::singleton()->get_setting('shop_email_orders').'">'.settings::singleton()->get_setting('shop_email_orders').'</a>';
//			die($content);
			//Send email
			common::email(array($email), settings::singleton()->get_setting('shop_email_orders'), 'Online Order', $content, true, settings::singleton()->get_setting('shop_email_orders'));
		}
	}
	
	public static function mark_order_paid($id)
	{
		database::singleton()->force_write_user();
		$result = database::singleton()->sql_update_by_id($id, 'tbl_orders', array('paid'), array(1));
		database::singleton()->destroy_write_user();
		return $result;
	}
	
	public static function mark_order_to_be_paid_by($id, $type)
	{
		database::singleton()->force_write_user();
		$result = database::singleton()->sql_update_by_id($id, 'tbl_orders', array('payment_method'), array($type));
		database::singleton()->destroy_write_user();
		return $result;
	}
}