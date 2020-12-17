package woo.app.clients;

import pt.tecnico.po.ui.Command;
import pt.tecnico.po.ui.DialogException;
import pt.tecnico.po.ui.Input;
import woo.Storefront;
import woo.app.exceptions.UnknownClientKeyException;
import woo.exceptions.NoSuchClientException;

/**
 * Show client.
 */
public class DoShowClient extends Command<Storefront> {

  /** Input field. */
  Input<String> _key;

  public DoShowClient(Storefront storefront) {
    super(Label.SHOW_CLIENT, storefront);
    _key = _form.addStringInput(Message.requestClientKey());
  }

  @Override
  public void execute() throws DialogException {
    try {
      _form.parse();
      _display.popup(_receiver.getClient(_key.value()));
    } catch (NoSuchClientException e) {
      throw new UnknownClientKeyException(e.getKey());
    }
  }
}
