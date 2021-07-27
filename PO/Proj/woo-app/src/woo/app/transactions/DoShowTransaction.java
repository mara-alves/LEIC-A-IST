package woo.app.transactions;

import pt.tecnico.po.ui.Command;
import pt.tecnico.po.ui.DialogException;
import pt.tecnico.po.ui.Input;
import woo.Storefront;
import woo.exceptions.NoSuchTransactionException;
import woo.app.exceptions.UnknownTransactionKeyException;

/**
 * Show specific transaction.
 */
public class DoShowTransaction extends Command<Storefront> {

  /** Input field. */
  Input<Integer> _key;

  public DoShowTransaction(Storefront receiver) {
    super(Label.SHOW_TRANSACTION, receiver);
    _key = _form.addIntegerInput(Message.requestTransactionKey());
  }

  @Override
  public final void execute() throws DialogException {
    try {
      _form.parse();
      _display.popup(_receiver.getTransaction(_key.value()));
    } catch (NoSuchTransactionException e) {
      throw new UnknownTransactionKeyException(e.getKey());
    }
  }

}
