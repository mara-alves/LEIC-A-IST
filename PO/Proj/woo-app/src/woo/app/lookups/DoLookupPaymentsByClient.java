package woo.app.lookups;

import pt.tecnico.po.ui.Command;
import pt.tecnico.po.ui.DialogException;
import pt.tecnico.po.ui.Input;
import woo.Storefront;
import woo.exceptions.NoSuchClientException;
import woo.app.exceptions.UnknownClientKeyException;

/**
 * Lookup payments by given client.
 */
public class DoLookupPaymentsByClient extends Command<Storefront> {

  /** Input field. */
  Input<String> _key;

  public DoLookupPaymentsByClient(Storefront storefront) {
    super(Label.PAID_BY_CLIENT, storefront);
    _key = _form.addStringInput(Message.requestClientKey());
  }

  @Override
  public void execute() throws DialogException {
    try {
      _form.parse();
      _display.popup(_receiver.getPayedTransactions(_key.value()));
    } catch (NoSuchClientException e) {
      throw new UnknownClientKeyException(e.getKey());
    }
  }

}
