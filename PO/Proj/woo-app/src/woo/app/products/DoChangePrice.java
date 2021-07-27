package woo.app.products;

import pt.tecnico.po.ui.Command;
import pt.tecnico.po.ui.DialogException;
import pt.tecnico.po.ui.Input;
import woo.Storefront;

/**
 * Change product price.
 */
public class DoChangePrice extends Command<Storefront> {

  /** Input fields. */
  Input<String> _key;
  Input<Integer> _price;
  
  public DoChangePrice(Storefront receiver) {
    super(Label.CHANGE_PRICE, receiver);
    _key = _form.addStringInput(Message.requestProductKey());
    _price = _form.addIntegerInput(Message.requestPrice());
  }

  @Override
  public final void execute() throws DialogException {
    _form.parse();
    _receiver.changePrice(_key.value(), _price.value());
  }
}
