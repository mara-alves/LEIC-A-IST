package woo.app.products;

import pt.tecnico.po.ui.Command;
import pt.tecnico.po.ui.DialogException;
import pt.tecnico.po.ui.Input;
import woo.Storefront;
import woo.app.exceptions.DuplicateProductKeyException;
import woo.app.exceptions.UnknownSupplierKeyException;
import woo.exceptions.RepeatedProductKeyException;
import woo.exceptions.NoSuchSupplierException;

/**
 * Register book.
 */
public class DoRegisterProductBook extends Command<Storefront> {

  /** Input fields. */
  Input<String> _key;
  Input<String> _title;
  Input<String> _author;
  Input<String> _isbn;
  Input<Integer> _price;
  Input<Integer> _criticalLevel;
  Input<String> _supplierKey;

  public DoRegisterProductBook(Storefront receiver) {
    super(Label.REGISTER_BOOK, receiver);
    _key = _form.addStringInput(Message.requestProductKey());
    _title = _form.addStringInput(Message.requestBookTitle());
    _author = _form.addStringInput(Message.requestBookAuthor());
    _isbn = _form.addStringInput(Message.requestISBN());
    _price = _form.addIntegerInput(Message.requestPrice());
    _criticalLevel = _form.addIntegerInput(Message.requestStockCriticalValue());
    _supplierKey = _form.addStringInput(Message.requestSupplierKey());
  }

  @Override
  public final void execute() throws DialogException {
    try {
      _form.parse();
      _receiver.registerBook(_key.value(),  _title.value(), _author.value(), _isbn.value(),
                               _supplierKey.value(), _price.value(), _criticalLevel.value(), 0);
    } catch (RepeatedProductKeyException e) {
        throw new DuplicateProductKeyException(e.getKey());
    } catch (NoSuchSupplierException e) {
      throw new UnknownSupplierKeyException(e.getKey());
    }
  }
}
