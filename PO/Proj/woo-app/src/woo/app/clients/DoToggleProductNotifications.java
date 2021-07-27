package woo.app.clients;

import pt.tecnico.po.ui.Command;
import pt.tecnico.po.ui.DialogException;
import pt.tecnico.po.ui.Input;
import woo.Storefront;
import woo.exceptions.NoSuchClientException;
import woo.exceptions.NoSuchProductException;
import woo.app.exceptions.UnknownClientKeyException;
import woo.app.exceptions.UnknownProductKeyException;

/**
 * Toggle product-related notifications.
 */
public class DoToggleProductNotifications extends Command<Storefront> {

  /** Input fields. */
  Input<String> _clientKey;
  Input<String> _productKey;

  public DoToggleProductNotifications(Storefront storefront) {
    super(Label.TOGGLE_PRODUCT_NOTIFICATIONS, storefront);
    _clientKey = _form.addStringInput(Message.requestClientKey());
    _productKey = _form.addStringInput(Message.requestProductKey());
  }

  @Override
  public void execute() throws DialogException {
    try {
      _form.parse();
      if (_receiver.toggleNotifications(_clientKey.value(), _productKey.value()) == true)
        _display.popup(Message.notificationsOn(_clientKey.value(), _productKey.value()));
      else
        _display.popup(Message.notificationsOff(_clientKey.value(), _productKey.value()));
    } catch (NoSuchClientException e) {
      throw new UnknownClientKeyException(e.getKey());
    } catch (NoSuchProductException e) {
      throw new UnknownProductKeyException(e.getKey());
    }
  }

}
