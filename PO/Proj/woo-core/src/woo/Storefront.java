package woo;

import java.io.IOException;
import java.io.FileNotFoundException;
import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import woo.exceptions.BadEntryException;
import woo.exceptions.MissingFileAssociationException;
import woo.exceptions.UnavailableFileException;
import woo.exceptions.ImportFileException;
import woo.exceptions.RepeatedClientKeyException;
import woo.exceptions.NoSuchClientException;
import woo.exceptions.RepeatedSupplierKeyException;
import woo.exceptions.NoSuchSupplierException;
import woo.exceptions.RepeatedProductKeyException;
import woo.exceptions.NoSuchServiceTypeException;
import woo.exceptions.NoSuchServiceLevelException;
import woo.exceptions.BadDateException;
import woo.exceptions.NoSuchProductException;
import woo.exceptions.NotEnoughProductException;
import woo.exceptions.NotThatSupplierException;
import woo.exceptions.TransactionsOffException;
import woo.exceptions.NoSuchTransactionException;


/**
 * Storefront: façade for the core classes.
 */
public class Storefront {

  /** Current filename. */
  private String _filename = "";

  /** The actual store. */
  private Store _store = new Store();

  /**
   * @throws IOException
   * @throws FileNotFoundException
   * @throws MissingFileAssociationException
   */
  public void save() throws IOException, FileNotFoundException, 
                     MissingFileAssociationException {
    if(_filename.equals(""))
      throw new MissingFileAssociationException();
    ObjectOutputStream out = new ObjectOutputStream(
                                new BufferedOutputStream(
                                  new FileOutputStream(_filename)));
    out.writeObject(_store);
    out.close();
  }

  /**
   * @param filename
   * @throws MissingFileAssociationException
   * @throws IOException
   * @throws FileNotFoundException
   */
  public void saveAs(String filename) throws MissingFileAssociationException,
        FileNotFoundException, IOException {
    _filename = filename;
    save();
  }

  /**
   * @param filename
   * @throws UnavailableFileException
   */
  public void load(String filename) throws UnavailableFileException {
    try {
      ObjectInputStream in = new ObjectInputStream(
                                new BufferedInputStream(
                                    new FileInputStream(filename)));
      _store = (Store)in.readObject();
      in.close();
      _filename = filename;
    } catch(IOException | ClassNotFoundException e) {
      throw new UnavailableFileException(filename);
    }
  }

  /**
   * @param textfile
   * @throws ImportFileException
   */
  public void importFile(String textfile) throws ImportFileException {
    try {
      _store.importFile(textfile);
    } catch (IOException | BadEntryException | RepeatedClientKeyException
            | RepeatedSupplierKeyException | RepeatedProductKeyException
            | NoSuchSupplierException | NoSuchServiceTypeException
            | NoSuchServiceLevelException e) {
      throw new ImportFileException(textfile);
    }
  }


  /** --------------------- DATE METHODS ---------------------- */

  /**
   * returns the current date
   */
  public int getDate() {
    return _store.getDate();
  }

  /**
   * advances date the requested number of days
   */
  public void advanceDate(int days) throws BadDateException {
    _store.advanceDate(days);
  }


  /** --------------------- BALANCE METHODS ---------------------- */

  /**
   * difference between sales and orders
   */
  public int getAvailableBalance() {
    return _store.getAvailableBalance();
  }

  /**
   * difference between sales and orders, but considering
   * sales that aren't payed yet too 
   */
  public int getContabilisticBalance() {
    return _store.getContabilisticBalance();
  }


  /** --------------------- CLIENT METHODS ----------------------- */

  /**
   * registers a client
   */
  public void registerClient(String key, String name, String address)
          throws RepeatedClientKeyException {
    _store.registerClient(key, name, address);
  }

   /**
    * return string of client and it's notifications
    */
  public String getClient(String key) throws NoSuchClientException {
    String res = _store.getClient(key).toString();
    res += _store.getClientNotifications(key);
    return res;
  }

  /** 
   * string with all clients
   */
  public String getClients() {
    String res = "";
    for (Client c: _store.getClients())
      res += c.toString() + "\n";
    return res;
  }

  /**
   * string with all client transactions 
   */
  public String getClientTransactions(String clientKey) 
          throws NoSuchClientException {
    String res = "";
    for (Transaction t: _store.getClientTransactions(clientKey))
      res += t.showToday(_store.getDate()) + "\n";
    return res;
  }
  
  /**
   * toggles client notifications for a certain product
   */
  public boolean toggleNotifications(String clientKey, String productKey)
          throws NoSuchClientException, NoSuchProductException {
    return _store.toggleNotifications(clientKey, productKey);
  }

  /**
   * pays a transaction
   */
  public void pay(int key) throws NoSuchTransactionException {
    _store.pay(key);
  }

  /** 
   * string with all payed transactions by the client
   */
  public String getPayedTransactions(String clientKey) 
          throws NoSuchClientException {
    String res = "";
    for (Sale s: _store.getPayedTransactions(clientKey))
      res += s.showToday(_store.getDate()) + "\n";
    return res;
  }


/** ---------------------- SUPPLIER METHODS ---------------------- */

  /**
   * registers a supplier
   */
  public void registerSupplier(String key, String name, String address) 
          throws RepeatedSupplierKeyException {
    _store.registerSupplier(key, name, address);
  }

  /**
   * string with all suppliers
   */
  public String getSuppliers() {
    String res = "";
    for (Supplier s: _store.getSuppliers())
      res += s.toString() + "\n";
    return res;
  }

  /**
   * toggles transactions 
   */
  public boolean toggleTransactions(String key) throws NoSuchSupplierException{
    return _store.toggleTransactions(key);
  }

  /**
   * string with all supplier transactions 
   */
  public String getSupplierTransactions(String supplierKey) 
          throws NoSuchSupplierException {
    String res = "";
    for (Transaction t: _store.getSupplierTransactions(supplierKey))
      res += t.showToday(_store.getDate()) + "\n";
    return res;
  }


/** -------------------------- PRODUCT METHODS -------------------------- */

  /**
   * registers a book
   */
  public void registerBook(String key, String title, String author, 
                           String isbn, String supplierKey, int price,
                           int criticalLevel, int stock) 
                           throws RepeatedProductKeyException, 
                           NoSuchSupplierException {
    _store.registerBook(key, title, author, isbn, supplierKey, price,
                        criticalLevel, stock);
  }

  /**
   * registers a box
   */
  public void registerBox(String key, String serviceType, String supplierKey,
                          int price, int criticalLevel, int stock) 
                          throws RepeatedProductKeyException,
                          NoSuchSupplierException, NoSuchServiceTypeException {
    _store.registerBox(key, serviceType, supplierKey, price,
                       criticalLevel, stock);
  }

  /**
   * registers a container
   */
  public void registerContainer(String key, String serviceType,
                                String serviceLevel, String supplierKey, 
                                int price, int criticalLevel, int stock)
                                throws RepeatedProductKeyException, 
                                NoSuchSupplierException,
                                NoSuchServiceTypeException,
                                NoSuchServiceLevelException {
    _store.registerContainer(key, serviceType, serviceLevel, supplierKey,
                             price, criticalLevel, stock);
  }

  /**
   * changes price of a product
   */
  public void changePrice(String key, int price) {
    _store.changePrice(key, price);
  }

  /**
   * string with all products
   */
  public String getProducts() {
    String res = "";
    for (Product p: _store.getProducts())
      res += p.toString() + "\n";
    return res;
  }

  /**
   * string with all products under certain price
   */
  public String getProductsUnderPrice(int price) {
    String res = "";
    for (Product p: _store.getProductsUnderPrice(price))
      res += p.toString() + "\n";
    return res;
  }


/** --------------------- TRANSACTIONS METHODS ---------------------- */

  /**
   * registers a sale associated with a client.
   */
  public void registerSale(String clientKey, int dateLimit,
                           String productKey, int quantity) 
                           throws NoSuchClientException,
                           NoSuchProductException,
                           NotEnoughProductException {
    _store.registerSale(clientKey, dateLimit, productKey, quantity);
  }

  /** 
   * registers an order
   */
  public void registerOrder(String supplierKey, String productKey,
                            int quantity) throws NoSuchSupplierException,
                            NoSuchProductException, TransactionsOffException,
                            NotThatSupplierException {
    _store.registerOrder(supplierKey, productKey, quantity);
  }

  /**
   * adds a product to last order
   */
  public void addToLastOrder(String supplierKey, String productKey,
                             int quantity) throws NoSuchProductException {
    _store.addToLastOrder(supplierKey, productKey, quantity);
  }

  /**
   * string with requested transaction
   */
  public String getTransaction(int id) throws NoSuchTransactionException {
    return _store.getTransaction(id).showToday(_store.getDate());
  }


}