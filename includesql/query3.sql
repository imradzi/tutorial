-- Sample Query 3: Product Catalog
-- Product management and search queries

CREATE TABLE IF NOT EXISTS products (
    id BINARY(16) PRIMARY KEY DEFAULT (ULID()),
    name VARCHAR(255) NOT NULL,
    description TEXT,
    price DECIMAL(10,2) NOT NULL,
    category_id BINARY(16),
    stock_quantity INT DEFAULT 0,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);

---- testing 
-- Product search and filtering queries
SELECT 
    p.id,
    p.name,
    p.description,
    p.price,
    p.stock_quantity
FROM products p 
WHERE p.stock_quantity > 0 
  AND p.price BETWEEN ? AND ?
ORDER BY p.created_at DESC
LIMIT 20;

-- Product category aggregation
SELECT 
    category_id,
    COUNT(*) as product_count,
    AVG(price) as avg_price,
    MIN(price) as min_price,
    MAX(price) as max_price
FROM products 
WHERE stock_quantity > 0
GROUP BY category_id
ORDER BY product_count DESC;-- Another comment for incremental test
