package woo.app.products;

import pt.tecnico.po.ui.Command;
import pt.tecnico.po.ui.DialogException;
import pt.tecnico.po.ui.Input;
import woo.Storefront;
import woo.app.exceptions.DuplicateProductKeyException;
import woo.app.exceptions.UnknownSupplierKeyException;
import woo.exceptions.RepeatedProductKeyException;
import woo.exceptions.NoSuchSupplierException;
import woo.exceptions.NoSuchServiceTypeException;
import woo.app.exceptions.UnknownServiceTypeException;
import woo.exceptions.NoSuchServiceLevelException;
import woo.app.exceptions.UnknownServiceLevelException;

/**
 * Register container.
 */
public class DoRegisterProductContainer extends Command<Storefront> {

  /** Input fields. */
  Input<String> _key;
  Input<String> _serviceType;
  Input<String> _serviceLevel;
  Input<Integer> _price;
  Input<Integer> _criticalLevel;
  Input<String> _supplierKey;

  public DoRegisterProductContainer(Storefront receiver) {
    super(Label.REGISTER_CONTAINER, receiver);
    _key = _form.addStringInput(Message.requestProductKey());
    _price = _form.addIntegerInput(Message.requestPrice());
    _criticalLevel = _form.addIntegerInput(Message.requestStockCriticalValue());
    _supplierKey = _form.addStringInput(Message.requestSupplierKey());
    _serviceType = _form.addStringInput(Message.requestServiceType());
    _serviceLevel = _form.addStringInput(Message.requestServiceLevel());
  }

  @Override
  public final void execute() throws DialogException {
    try {
      _form.parse();
      _receiver.registerContainer(_key.value(), _serviceType.value(), _serviceLevel.value(), _supplierKey.value(), _price.value(), _criticalLevel.value(), 0);
    } catch (RepeatedProductKeyException e) {
        throw new DuplicateProductKeyException(e.getKey());
    } catch (NoSuchSupplierException e) {
      throw new UnknownSupplierKeyException(e.getKey());
    } catch (NoSuchServiceTypeException e) {
      throw new UnknownServiceTypeException(e.getServiceType());
    } catch (NoSuchServiceLevelException e) {
      throw new UnknownServiceLevelException(e.getServiceLevel());
    }
  }
}
