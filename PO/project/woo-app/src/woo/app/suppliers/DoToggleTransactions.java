package woo.app.suppliers;

import pt.tecnico.po.ui.Command;
import pt.tecnico.po.ui.DialogException;
import pt.tecnico.po.ui.Input;
import woo.Storefront;
import woo.app.exceptions.UnknownSupplierKeyException;
import woo.exceptions.NoSuchSupplierException;

/**
 * Enable/disable supplier transactions.
 */
public class DoToggleTransactions extends Command<Storefront> {

  /** Input field. */
  Input<String> _key;

  public DoToggleTransactions(Storefront receiver) {
    super(Label.TOGGLE_TRANSACTIONS, receiver);
    _key = _form.addStringInput(Message.requestSupplierKey());
  }

  @Override
  public void execute() throws DialogException {
    try {
      _form.parse();
      if (_receiver.toggleTransactions(_key.value()) == true)
        _display.popup(Message.transactionsOn(_key.value()));
      else
        _display.popup(Message.transactionsOff(_key.value()));
    } catch(NoSuchSupplierException e) {
      throw new UnknownSupplierKeyException(e.getKey());
    }
  }

}
