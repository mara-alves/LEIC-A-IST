package woo.app.suppliers;

import pt.tecnico.po.ui.Command;
import pt.tecnico.po.ui.DialogException;
import pt.tecnico.po.ui.Input;
import woo.Storefront;
import woo.app.exceptions.DuplicateSupplierKeyException;
import woo.exceptions.RepeatedSupplierKeyException;

/**
 * Register supplier.
 */
public class DoRegisterSupplier extends Command<Storefront> {

  /** Input fields. */
  Input<String> _key;
  Input<String> _name;
  Input<String> _address;

  public DoRegisterSupplier(Storefront receiver) {
    super(Label.REGISTER_SUPPLIER, receiver);
    _key = _form.addStringInput(Message.requestSupplierKey());
    _name = _form.addStringInput(Message.requestSupplierName());
    _address = _form.addStringInput(Message.requestSupplierAddress());
  }

  @Override
  public void execute() throws DialogException {
    _form.parse();
    try {
        _receiver.registerSupplier(_key.value(), _name.value(), _address.value());
    }
    catch (RepeatedSupplierKeyException e) {
        throw new DuplicateSupplierKeyException(e.getKey());
    }
  }

}
