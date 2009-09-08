<?php
class cmspage_cart extends cmspage_html
{
	public function initalise()
	{
		$this->value = CMS::Singleton()->path_vars(1);
		return parent::initalise();
	}

	public function run()
	{
		//Check cookies are enabled first
		Common::cookie_check();
		
		switch (CMS::Singleton()->path_vars(1))
		{
			case 'add':
				$quantity = get::variable('quantity', 'numeric');
				if(!$quantity)
				{
					$quantity = 1;
				}
				shoppingcart::add_to_cart(CMS::Singleton()->path_vars(2), 1);
				common::bounce_to_referer();
				break;

			case 'update':
				shoppingcart::update_cart_object(CMS::Singleton()->path_vars(2), get::variable('quantity', 'numeric'));
				common::redirect('cart');
				break;
				
			case 'termsandconditions':
				common::redirect('tandc');
				break;
				
			case 'payment':
				if(is_numeric($_SESSION['order']['id']))
				{
					$type = 'Payment';
					if(CMS::Singleton()->path_vars(2))
					{
						$type = strtolower(CMS::Singleton()->path_vars(2));
						shoppingcart::mark_order_to_be_paid_by($_SESSION['order']['id'], $type);
						$type = 'element_payment_'.$type;
					
						$element = Element::type('element_payment_thankyou');
						$element->build('element_payment_thankyou');
//						unset($_SESSION['order']);
					}
					
					$element = Element::type($type);
					$element->build($type);
					Render::Singleton()->render_html(Content::Singleton()->build());
				}
				break;
				
			case 'place_order':
				if($this->check_form())
				{
					$details = array(
						'first_name' => post::variable('first_name', 'string'),
						'last_name' => post::variable('last_name', 'string'),
						'address1' => post::variable('address1', 'string'),
						'city' => post::variable('city', 'string'),
						'county' => post::variable('county', 'string'),
						'phone' => post::variable('phone', 'string'),
						'email' => post::variable('email'),
						'postcode' => post::variable('postcode'),
					);
					
					if(post::variable('address2', 'isset'))
					{
						$details['address2'] = post::variable('address2', 'string');
					}
					
					if(shoppingcart::finalise(post::variable('email'), $details))
					{
						common::redirect('cart/payment');
					}
					error::error('Error placing your order, the cart was empty or there was a server problem, please try again latter');
				}
				common::redirect('cart/checkout');
				
			case 'checkout':
				if(shoppingcart::get_number_of_items())
				{
					$element = Element::type('Checkout');
					$element->build('Checkout');
					Render::Singleton()->render_html(Content::Singleton()->build());
					break;
				}
				common::redirect('cart');
				break;

			default:
				//Show the cart
				$element = Element::type('elementCart');
				$element->build('elementCart');
				Render::Singleton()->render_html(Content::Singleton()->build());
				break;
		}
	}
	
	private function check_form()
	{
		if(strlen(post::variable('first_name', 'string')) > 1 && strlen(post::variable('last_name', 'string')) > 1 && strlen(post::variable('address1', 'string')) > 1 && strlen(post::variable('city', 'string')) > 1 && strlen(post::variable('county', 'string')) > 1 && strlen(post::variable('phone', 'string')) > 1 && EmailAddressValidator::check_email_address(post::variable('email')) )
		{
			return true;
		}
		return false;
	}
}
?>