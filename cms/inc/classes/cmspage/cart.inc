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
				die('Terms and conditions');
				break;
				
			case 'payment':
				die('Terms and conditions');
				break;
				
			case 'checkout':
				if(shoppingcart::get_number_of_items())
				{
					$element = Element::type('elementCheckout');
					$element->build('elementCheckout');
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
}
?>