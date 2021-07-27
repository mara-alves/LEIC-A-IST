package woo;

import java.io.Serializable;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.Collection;
import java.util.Collections;
import java.util.Map;
import java.util.NavigableMap;
import java.util.TreeMap;
import java.util.List;
import java.util.ArrayList;
import woo.exceptions.BadEntryException;
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
 * Class Store implements a store.
 */
public class Store implements Serializable {

  /** Serial number for serialization. */
  private static final long serialVersionUID = 202009192006L;

  /** Products. */
  private Map<String, Product> _products = new TreeMap<String, Product>(
                                            String.CASE_INSENSITIVE_ORDER);
  /** Clients. */
  private Map<String, Client> _clients = new TreeMap<String, Client>(
                                            String.CASE_INSENSITIVE_ORDER);
  /** Suppliers. */
  private Map<String, Supplier> _suppliers = new TreeMap<String, Supplier>(
                                                String.CASE_INSENSITIVE_ORDER);
  /** Transactions. */
  private NavigableMap<Integer, Transaction> _transactions = new TreeMap<Integer, Transaction>();
  
  /** Number of transactions. */
  private int _count = 0;

  /** Date. */
  private int _date = 0;


  /**
   * @param txtfile filename to be loaded.
   * @throws IOException
   * @throws BadEntryException
   */
  void importFile(String txtfile) throws IOException, BadEntryException, 
                                  RepeatedClientKeyException, 
                                  RepeatedSupplierKeyException,
                                  RepeatedProductKeyException,
                                  NoSuchSupplierException,
                                  NoSuchServiceTypeException,
                                  NoSuchServiceLevelException {
    BufferedReader reader = new BufferedReader(new FileReader(txtfile));
    String line;
    while ((line = reader.readLine()) != null) {
      String[] fields = line.split("\\|");
      if (fields[0].equals("CLIENT"))
          registerClient(fields[1], fields[2], fields[3]);
      else if (fields[0].equals("SUPPLIER"))
          registerSupplier(fields[1], fields[2], fields[3]);
      else if (fields[0].equals("BOOK"))
          registerBook(fields[1], fields[2], fields[3], fields[4], fields[5],
                       Integer.parseInt(fields[6]), 
                       Integer.parseInt(fields[7]),
                       Integer.parseInt(fields[8]));
      else if (fields[0].equals("BOX"))
          registerBox(fields[1], fields[2], fields[3],
                      Integer.parseInt(fields[4]),
                      Integer.parseInt(fields[5]),
                      Integer.parseInt(fields[6]));
      else if (fields[0].equals("CONTAINER"))
          registerContainer(fields[1], fields[2], fields[3], fields[4], 
                            Integer.parseInt(fields[5]), 
                            Integer.parseInt(fields[6]),
                            Integer.parseInt(fields[7]));
    }
  }


  /** ----------------------- DATE METHODS ----------------------- */

  /**
   * @return the current date
   */
  public int getDate() {
    return _date;
  }

  /**
   * Advances date.
   *
   * @param days
   *        number of days to advance
   * @throws BadDateException
   */
  public void advanceDate(int days) throws BadDateException {
    if(days <= 0)
      throw new BadDateException(days);
    _date += days;
  }


  /** --------------------- BALANCE METHODS ---------------------- */

  /**
   * @return the available balance
   *         difference between sales and orders
   */
  public int getAvailableBalance() {
    double res = 0;
    for(Transaction t : _transactions.values()) {
      res += t.payedValue();
    }
    return (int)res;
  }

  /**
   * @return the contabilistic balance
   *         difference between sales and orders, but considering
   *         sales that aren't payed yet too
   */
  public int getContabilisticBalance() {
    double res = 0;
    for(Transaction t : _transactions.values()) {
      res += t.todaysValue(getDate());
    }
    return (int)res;
  }


  /** ---------------------- CLIENT METHODS ---------------------- */

  /**
   * Create and register client.
   * 
   * @param key
   *          client's key
   * @param name
   *          client's name
   * @param address
   *          client's address
   * @throws RepeatedClientKeyException
   */
  public void registerClient(String key, String name, String address) 
          throws RepeatedClientKeyException {
    if (_clients.get(key) != null)
      throw new RepeatedClientKeyException(key);

    Client client = new Client(key, name, address);
    _clients.put(client.getKey(), client);

    /* Enable notifications for all existent products */
    for(Product p : _products.values()) {
      p.registerObserver(client.getKey(), client);
    }
  }

  /**
   * Get the client with the given String.
   * 
   * @param key
   *          the client key.
   * 
   * @return the client.
   * @throws NoSuchClientException
   */
  public final Client getClient(String key) throws NoSuchClientException {
    if (_clients.get(key) == null)
      throw new NoSuchClientException(key);
    return _clients.get(key);
  }

  /**
   * Return all clients as an unmodifiable collection.
   * 
   * @return a collection with all clients.
   */
  public Collection<Client> getClients() {
    return Collections.unmodifiableCollection(_clients.values());
  }

  /**
   * Return a string with the client notifications
   * and deletes them after that.
   *
   * @param clientKey
   *        Key of client
   * @return string with the client notifications
   */
  public String getClientNotifications(String clientKey) 
          throws NoSuchClientException {
    Client client = getClient(clientKey);
    String res = "";
    for(String notification : client.getNotifications())
      res += "\n" + notification;
    client.getNotifications().clear();
    return res;
  }

  /**
   * Return all client transactions.
   * 
   * @param clientKey
   *          key of requested client
   * @return a collection with all client transactions.
   */
  public Collection<Transaction> getClientTransactions(String clientKey)
          throws NoSuchClientException {
    Client client = _clients.get(clientKey);
    if(client == null)
      throw new NoSuchClientException(clientKey);
    return Collections.unmodifiableCollection(client.getSales());
  }

  /**
   * Toggles client notifications for a certain product.
   *
   * @param clientKey
   *          key of client
   * @param productKey
   *          key of product
   * @throws NoSuchClientException
   * @throws NoSuchProductException
   */
  public boolean toggleNotifications(String clientKey, String productKey)
          throws NoSuchClientException, NoSuchProductException {
    Client client = _clients.get(clientKey);
    if(client == null)
      throw new NoSuchClientException(clientKey);
      
    Product product = _products.get(productKey);
    if(product == null)
      throw new NoSuchProductException(productKey);

    return client.toggleNotifications(product);
  }

  /**
   * Pays a transaction.
   *
   * @param key
   *          key of transaction
   * @throws NoSuchTransactionException
   */
  public void pay(int key) throws NoSuchTransactionException {
    Transaction transaction = _transactions.get(key);
    if(transaction == null)
      throw new NoSuchTransactionException(key);
    transaction.pay(getDate());
  }

  /**
   * Collection with all payed transactions by a client.
   * (these transactions must be sales)
   *
   * @param key
   *          key of transaction
   * @throws NoSuchClientException
   */
  public Collection<Sale> getPayedTransactions(String clientKey)
          throws NoSuchClientException {
    Client client = _clients.get(clientKey);
    if(client == null)
      throw new NoSuchClientException(clientKey);
    
    ArrayList<Sale> payed = new ArrayList<Sale>();
    for(Sale s : client.getSales()) {
      if(s.isPayed())
        payed.add(s);
    }
    return Collections.unmodifiableCollection(payed);
  }


/** ----------------------- SUPPLIER METHODS --------------------- */

  /**
   * Create and register supplier.
   * 
   * @param key
   *          supplier's key
   * @param name
   *          supplier's name
   * @param address
   *          supplier's address
   * @throws RepeatedSupplierKeyException
   */
  public void registerSupplier(String key, String name, String address)
          throws RepeatedSupplierKeyException {
    if (_suppliers.get(key) != null)
      throw new RepeatedSupplierKeyException(key);
    Supplier supplier = new Supplier(key, name, address);
    _suppliers.put(key, supplier);
  }

  /**
   * Return all suppliers as an unmodifiable collection.
   * 
   * @return a collection with all clients.
   */
  public Collection<Supplier> getSuppliers() {
    return Collections.unmodifiableCollection(_suppliers.values());
  }

  /**
   * Toggles supplier transactions
   *
   * @param key
   *          supplier key
   * @throws NoSuchSupplierException
   */
  public boolean toggleTransactions(String key) 
          throws NoSuchSupplierException {
    Supplier supplier =  _suppliers.get(key);
    if (supplier == null)
      throw new NoSuchSupplierException(key);
    return supplier.toggleTransactions();
  }

  /**
   * Return all supplier transactions.
   * 
    * @param supplierKey
   *          key of requested client
   * @return a collection with all supplier transactions.
   */
  public Collection<Transaction> getSupplierTransactions(String supplierKey)
          throws NoSuchSupplierException {
    Supplier supplier = _suppliers.get(supplierKey);
    if(supplier == null)
      throw new NoSuchSupplierException(supplierKey);
    return Collections.unmodifiableCollection(supplier.getOrders());
  }


/** ---------------------- PRODUCTS METHODS ---------------------- */

  /**
   * Create and register book.
   * 
   * @param key
   *          product key
   * @param title
   *          book title
   * @param author
   *          book author
   * @param isbn
   *          book isbn
   * @param supplierKey
   *          key of the supplier associated with the book
   * @param price
   *          book price
   * @param criticalLevel
   *          product's critical level
   * @param stock
   *          quantity of product in stock
   * @throws RepeatedProductKeyException
   * @throws NoSuchSupplierException
   */
  public void registerBook(String key, String title, String author, 
                           String isbn, String supplierKey, int price,
                           int criticalLevel, int stock) 
                           throws RepeatedProductKeyException, 
                           NoSuchSupplierException {
    if (_suppliers.get(supplierKey) == null)
      throw new NoSuchSupplierException(supplierKey);

    if (_products.get(key) != null)
      throw new RepeatedProductKeyException(key);

    Book book = new Book(key, title, author, isbn, 
                        _suppliers.get(supplierKey), price,
                        criticalLevel, stock);

    _products.put(key, book);
    /* By default, all clients have enabled notifications */
    book.registerObservers(_clients);
  }

  /**
   * Create and register box.
   * 
   * @param key
   *          product key
   * @param serviceType
   *          service type
   * @param supplierKey
   *          key of the supplier associated with the box
   * @param price
   *          box price
   * @param criticalLevel
   *          product's critical level
   * @param stock
   *          quantity of product in stock
   * @throws RepeatedProductKeyException
   * @throws NoSuchSupplierException
   */
  public void registerBox(String key, String serviceType, String supplierKey,
                          int price, int criticalLevel, int stock) 
                          throws RepeatedProductKeyException,
                          NoSuchSupplierException, NoSuchServiceTypeException{
    if (_suppliers.get(supplierKey) == null)
      throw new NoSuchSupplierException(supplierKey);

    if (_products.get(key) != null)
      throw new RepeatedProductKeyException(key);

    if (!serviceType.matches("NORMAL|AIR|EXPRESS|PERSONAL"))
      throw new NoSuchServiceTypeException(serviceType);

    Box box = new Box(key, serviceType, 
                      _suppliers.get(supplierKey), price,
                      criticalLevel, stock);

    _products.put(key, box);
    box.registerObservers(_clients);
  }

  /**
   * Create and register container.
   * 
   * @param key
   *          product key
   * @param serviceType
   *          service type
   * @param serviceLevel
   *          service level
   * @param supplierKey
   *          key of the supplier associated with the container
   * @param price
   *          container price
   * @param criticalLevel
   *          product's critical level
   * @param stock
   *          quantity of product in stock
   * @throws RepeatedProductKeyException
   * @throws NoSuchSupplierException
   */
  public void registerContainer(String key, String serviceType, 
                                String serviceLevel, String supplierKey, 
                                int price, int criticalLevel, int stock) 
                                throws RepeatedProductKeyException,
                                NoSuchSupplierException, 
                                NoSuchServiceTypeException,
                                NoSuchServiceLevelException {

    if (_suppliers.get(supplierKey) == null)
      throw new NoSuchSupplierException(supplierKey);

    if (_products.get(key) != null)
      throw new RepeatedProductKeyException(key);

    if (!serviceType.matches("NORMAL|AIR|EXPRESS|PERSONAL"))
      throw new NoSuchServiceTypeException(serviceType);

    if (!serviceLevel.matches("B4|C4|C5|DL"))
      throw new NoSuchServiceLevelException(serviceLevel);

    Container container = new Container(key, serviceType,
                              serviceLevel, _suppliers.get(supplierKey), 
                              price, criticalLevel, stock);

    _products.put(key, container);
    container.registerObservers(_clients);
  }

  /**
   * Change price of a product.
   *
   * @param key
   *          product key
   * @param price
   *          new price for product
   */
  public void changePrice(String key, int price) {
    if(price > 0 && _products.get(key) != null) {
      _products.get(key).setPrice(price);
    }
  }

  /**
   * Return all products as an unmodifiable collection.
   * 
   * @return a collection with all products.
   */
  public Collection<Product> getProducts() {
    return Collections.unmodifiableCollection(_products.values());
  }

  /**
   * Return all products under a certain price.
   * 
   * @param price
              max price of products
   * @return a collection with products.
   */
  public Collection<Product> getProductsUnderPrice(int price) {
    ArrayList<Product> products = new ArrayList<Product>();
    for(Product p : getProducts()) {
      if(p.getPrice() < price)
        products.add(p);
    }
    return Collections.unmodifiableCollection(products);
  }



/** --------------------- TRANSACTIONS METHODS ------------------- */

  /**
   * Registers a sale associated with a client.
   * 
   * @param clientKey
   *          client key
   * @param dateLimit
   *          payment deadline
   * @param productKey
   *          product key
   * @param quantity
   *          amout of product being sold
   * @throws NoSuchClientException
   * @throws NoSuchProductException
   * @throws NotEnoughProductException
   */
  public void registerSale(String clientKey, int dateLimit,
                           String productKey, int quantity)
                           throws NoSuchClientException,
                           NoSuchProductException,
                           NotEnoughProductException {
    Client client =  _clients.get(clientKey);
    if(client == null)
      throw new NoSuchClientException(clientKey);

    Product product = _products.get(productKey);
    if(product == null)
      throw new NoSuchProductException(productKey);

    if(quantity > product.getStock())
      throw new NotEnoughProductException(productKey, product.getStock());
    
    Sale sale = new Sale(_count, product, quantity, dateLimit, client);
    _count++;

    client.registerSale(sale);
    _transactions.put(sale.getKey(), sale);
    product.removeStock(quantity);
  }

  /** 
   * Registers an order associated with a supplier
   *
   * @param supplierKey
   *          supplier key
   * @param productKey
   *          first product in the order
   * @param quantity
   *          amount of product
   * @return the order id
   */
  public void registerOrder(String supplierKey, String productKey, 
                            int quantity) throws NoSuchSupplierException, 
                            NoSuchProductException, TransactionsOffException,
                            NotThatSupplierException {

    Supplier supplier = _suppliers.get(supplierKey);
    if(supplier == null)
      throw new NoSuchSupplierException(supplierKey);

    Product product = _products.get(productKey);
    if(product == null)
      throw new NoSuchProductException(productKey);
    
    if(!supplier.isActive())
      throw new TransactionsOffException(supplierKey);

    if(!product.getSupplierKey().equals(supplierKey))
      throw new NotThatSupplierException(supplierKey, productKey);

    Order order = new Order(_count, product, quantity, supplierKey, getDate());
    _count++;
    
    supplier.registerOrder(order);
    _transactions.put(order.getKey(), order);
    product.addStock(quantity);
  }

  /** 
   * Adds a product to the most recent order associated with supplier
   *
   * @param supplierKey
   *          supplier key
   * @param productKey
   *          product to be added
   * @param quantity
   *          amount of product
   * @return the order id
   */
  public void addToLastOrder(String supplierKey, String productKey,
                             int quantity) throws NoSuchProductException {
    Supplier supplier = _suppliers.get(supplierKey);
    Product product = _products.get(productKey);

    /* if product is invalid, cancel order */
    if(product == null) {
      supplier.cancelLastOrder();
      _transactions.pollLastEntry();
      _count--;
      throw new NoSuchProductException(productKey);
    }

    supplier.addToLastOrder(product, quantity);
    product.addStock(quantity);
  }

  /**
   * Get the transaction with the given id.
   * 
   * @param key
   *          the transaction id.
   * 
   * @return the transaction.
   * @throws NoSuchTransactionException
   */
  public final Transaction getTransaction(int key) 
                           throws NoSuchTransactionException {
    if (_transactions.get(key) == null)
      throw new NoSuchTransactionException(key);
    return _transactions.get(key);
  }

}