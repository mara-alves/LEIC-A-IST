package woo.app.transactions;

import pt.tecnico.po.ui.Command;
import pt.tecnico.po.ui.DialogException;
import pt.tecnico.po.ui.Input;
import woo.Storefront;
import woo.exceptions.NoSuchTransactionException;
import woo.app.exceptions.UnknownTransactionKeyException;

/**
 * Pay transaction (sale).
 */
public class DoPay extends Command<Storefront> {

  /** Input fields. */
  Input<Integer> _key;
  
  public DoPay(Storefront storefront) {
    super(Label.PAY, storefront);
    _key = _form.addIntegerInput(Message.requestTransactionKey());
  }

  @Override
  public final void execute() throws DialogException {
    try {
      _form.parse();
      _receiver.pay(_key.value());
    } catch (NoSuchTransactionException e) {
      throw new UnknownTransactionKeyException(e.getKey());
    }
  }

}
