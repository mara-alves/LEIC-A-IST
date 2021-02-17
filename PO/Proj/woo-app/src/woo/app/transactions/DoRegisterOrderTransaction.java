package woo.app.transactions;

import pt.tecnico.po.ui.Command;
import pt.tecnico.po.ui.DialogException;
import pt.tecnico.po.ui.Input;
import woo.Storefront;
import woo.app.exceptions.UnknownProductKeyException;
import woo.app.exceptions.UnknownSupplierKeyException;
import woo.app.exceptions.UnauthorizedSupplierException;
import woo.app.exceptions.WrongSupplierException;
import woo.exceptions.NoSuchProductException;
import woo.exceptions.NoSuchSupplierException;
import woo.exceptions.TransactionsOffException;
import woo.exceptions.NotThatSupplierException;

/**
 * Register order.
 */
public class DoRegisterOrderTransaction extends Command<Storefront> {

  /** Input fields. */
  Input<String> _supplierKey;
  Input<String> _productKey;
  Input<Integer> _quantity;
  Input<Boolean> _more;

  String _skey;

  public DoRegisterOrderTransaction(Storefront receiver) {
    super(Label.REGISTER_ORDER_TRANSACTION, receiver);
  }

  @Override
  public final void execute() throws DialogException {
    try {
      _form.clear();
      _supplierKey = _form.addStringInput(Message.requestSupplierKey());
      _productKey = _form.addStringInput(Message.requestProductKey());
      _quantity = _form.addIntegerInput(Message.requestAmount());
      _more = _form.addBooleanInput(Message.requestMore());
      _form.parse();
      _receiver.registerOrder(_supplierKey.value(), _productKey.value(), _quantity.value());
      
      _skey = _supplierKey.value();

      while(_more.value() == true) {
        _form.clear();
        _productKey = _form.addStringInput(Message.requestProductKey());
        _quantity = _form.addIntegerInput(Message.requestAmount());
        _more = _form.addBooleanInput(Message.requestMore());
        _form.parse();
        _receiver.addToLastOrder(_skey, _productKey.value(), _quantity.value());
      }
    } catch(NoSuchSupplierException e) {
      throw new UnknownSupplierKeyException(e.getKey());
    } catch(NoSuchProductException e) {
      throw new UnknownProductKeyException(e.getKey());
    } catch(TransactionsOffException e) {
      throw new UnauthorizedSupplierException(e.getKey());
    } catch(NotThatSupplierException e) {
      throw new WrongSupplierException(e.getSupplierKey(), e.getProductKey());
    }
  }

}
