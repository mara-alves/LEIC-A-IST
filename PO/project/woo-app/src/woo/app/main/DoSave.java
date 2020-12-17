package woo.app.main;

import pt.tecnico.po.ui.Command;
import pt.tecnico.po.ui.DialogException;
import pt.tecnico.po.ui.Input;
import woo.Storefront;
import woo.exceptions.MissingFileAssociationException;
import java.io.IOException;
import java.io.FileNotFoundException;

/**
 * Save current state to file under current name (if unnamed, query for name).
 */
public class DoSave extends Command<Storefront> {

  /** Input field. */
  Input<String> _filename;

  /** @param receiver */
  public DoSave(Storefront receiver) {
    super(Label.SAVE, receiver);
    _filename = _form.addStringInput(Message.newSaveAs());
  }

  /** @see pt.tecnico.po.ui.Command#execute() */
  @Override
  public final void execute() {
    try {
      _receiver.save();
    } catch(MissingFileAssociationException mfa) {
      try {
        _form.parse();
        _receiver.saveAs(_filename.value());
      } catch(MissingFileAssociationException | IOException e) {
        e.printStackTrace();
      }
    } catch(IOException ioe) {
      ioe.printStackTrace();
    }
  }
}
