package woo.app.transactions;

import pt.tecnico.po.ui.Command;
import pt.tecnico.po.ui.DialogException;
import pt.tecnico.po.ui.Input;
import woo.Storefront;
import woo.app.exceptions.UnavailableProductException;
import woo.app.exceptions.UnknownClientKeyException;
import woo.app.exceptions.UnknownProductKeyException;
import woo.exceptions.NotEnoughProductException;
import woo.exceptions.NoSuchClientException;
import woo.exceptions.NoSuchProductException;

/**
 * Register sale.
 */
public class DoRegisterSaleTransaction extends Command<Storefront> {

  /** Input fields. */
  Input<String> _clientKey;
  Input<Integer> _dateLimit;
  Input<String> _productKey;
  Input<Integer> _quantity;

  public DoRegisterSaleTransaction(Storefront receiver) {
    super(Label.REGISTER_SALE_TRANSACTION, receiver);
    _clientKey = _form.addStringInput(Message.requestClientKey());
    _dateLimit = _form.addIntegerInput(Message.requestPaymentDeadline());
    _productKey = _form.addStringInput(Message.requestProductKey());
    _quantity = _form.addIntegerInput(Message.requestAmount());
  }

  @Override
  public final void execute() throws DialogException {
    try {
      _form.parse();
      _receiver.registerSale(_clientKey.value(), _dateLimit.value(), _productKey.value(), _quantity.value());
    } catch (NotEnoughProductException e) {
      throw new UnavailableProductException(e.getKey(), _quantity.value(), e.getAvailable());
    } catch (NoSuchClientException e) {
      throw new UnknownClientKeyException(e.getKey());
    } catch (NoSuchProductException e) {
      throw new UnknownProductKeyException(e.getKey());
    }
  }

}
