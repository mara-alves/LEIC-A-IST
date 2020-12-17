package woo;

import java.io.Serializable;

public class Book extends Product implements Serializable {
  /** Book title */
  private String _title;
  /** Book author */
  private String _author;
  /** Book ISBN */
  private String _isbn;

  /**
   * Constructor for class Book.
   * 
   * @param key
   *          product key
   * @param supplier
   *          product's supplier
   * @param price
   *          price of product
   * @param criticalLevel
   *          critical value of product
   * @param title
   *          book title
   * @param author
   *          book author
   * @param isbn
   *          book isbn
   * @param stock
   *          current stock of book
   */
  public Book(String key, String title, String author, String isbn,
              Supplier supplier, int price, int criticalLevel, int stock) {
    super(3, key, supplier, price, criticalLevel, stock);
    _title = title;
    _author = author;
    _isbn = isbn;
  }

  /** 
   * @return string of product
   */
  public String toString() {
    return "BOOK"+"|"+super.getKey()+"|"+super.getSupplierKey()+"|"
            +super.getPrice()+"|"+super.getCriticalLevel()
            +"|"+super.getStock()+"|"+_title+"|"+_author+"|"+_isbn;
  }

}