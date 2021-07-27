package woo.app.clients;

import pt.tecnico.po.ui.Command;
import pt.tecnico.po.ui.DialogException;
import pt.tecnico.po.ui.Input;
import woo.Storefront;
import woo.exceptions.NoSuchClientException;
import woo.app.exceptions.UnknownClientKeyException;

/**
 * Show all transactions for a specific client.
 */
public class DoShowClientTransactions extends Command<Storefront> {

  /** Input field. */
  Input<String> _key;

  public DoShowClientTransactions(Storefront storefront) {
    super(Label.SHOW_CLIENT_TRANSACTIONS, storefront);
    _key = _form.addStringInput(Message.requestClientKey());
  }

  @Override
  public void execute() throws DialogException {
    try {
      _form.parse();
      _display.popup(_receiver.getClientTransactions(_key.value()));
    } catch (NoSuchClientException e) {
      throw new UnknownClientKeyException(e.getKey());
    }
  }

}
