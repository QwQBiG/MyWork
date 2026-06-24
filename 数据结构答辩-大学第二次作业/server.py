# iqwqi: Python API server - bridges frontend <-> MySQL
import http.server
import json
import os
import urllib.parse
import pymysql

DB_CONFIG = {
    'host': 'localhost',
    'user': 'root',
    'password': 'zbh123456',
    'database': 'kfc_pos',
    'port': 3306,
    'charset': 'gbk'
}

JSON_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'menu_data.json')


def get_db():
    return pymysql.connect(**DB_CONFIG)


def query_all_menu():
    """SELECT all menu items from MySQL, return list of dicts."""
    conn = get_db()
    try:
        with conn.cursor() as cur:
            cur.execute("SELECT category_id, food_id, food_name, price FROM menu ORDER BY category_id, food_id")
            rows = cur.fetchall()
            return [{"categoryId": r[0], "id": r[1], "name": r[2], "price": float(r[3])} for r in rows]
    finally:
        conn.close()


def regenerate_json():
    """Write menu_data.json from MySQL for frontend direct fetch."""
    data = query_all_menu()
    with open(JSON_PATH, 'w', encoding='utf-8') as f:
        json.dump(data, f, ensure_ascii=False, indent=2)


def add_food(category_id, food_id, name, price):
    conn = get_db()
    try:
        with conn.cursor() as cur:
            cur.execute(
                "INSERT INTO menu (category_id, food_id, food_name, price) VALUES (%s, %s, %s, %s)",
                (category_id, food_id, name, price)
            )
        conn.commit()
        regenerate_json()
        return True, "ok"
    except pymysql.err.IntegrityError:
        return False, "duplicate food_id"
    except Exception as e:
        return False, str(e)
    finally:
        conn.close()


def update_food(category_id, food_id, name, price):
    conn = get_db()
    try:
        with conn.cursor() as cur:
            cur.execute(
                "UPDATE menu SET category_id=%s, food_name=%s, price=%s WHERE food_id=%s",
                (category_id, name, price, food_id)
            )
        conn.commit()
        regenerate_json()
        return cur.rowcount > 0, "not found" if cur.rowcount == 0 else "ok"
    except Exception as e:
        return False, str(e)
    finally:
        conn.close()


def delete_food(food_id):
    conn = get_db()
    try:
        with conn.cursor() as cur:
            cur.execute("DELETE FROM menu WHERE food_id=%s", (food_id,))
        conn.commit()
        regenerate_json()
        return cur.rowcount > 0, "not found" if cur.rowcount == 0 else "ok"
    except Exception as e:
        return False, str(e)
    finally:
        conn.close()


class APIHandler(http.server.BaseHTTPRequestHandler):
    def log_message(self, format, *args):
        print(f"[iqwqi] {args[0]}")

    def send_json(self, data, status=200):
        body = json.dumps(data, ensure_ascii=False).encode('utf-8')
        self.send_response(status)
        self.send_header('Content-Type', 'application/json; charset=utf-8')
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Content-Length', str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def do_OPTIONS(self):
        self.send_response(200)
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET,POST,PUT,DELETE,OPTIONS')
        self.send_header('Access-Control-Allow-Headers', 'Content-Type')
        self.end_headers()

    def do_GET(self):
        parsed = urllib.parse.urlparse(self.path)
        path = parsed.path
        print(f"[iqwqi] GET {path}")

        try:
            if path == '/':
                self.send_response(200)
                self.send_header('Content-Type', 'text/html; charset=utf-8')
                self.end_headers()
                with open('index.html', 'rb') as f:
                    self.wfile.write(f.read())
                return

            if path == '/menu_data.json':
                regenerate_json()
                self.send_response(200)
                self.send_header('Content-Type', 'application/json; charset=utf-8')
                self.end_headers()
                with open(JSON_PATH, 'rb') as f:
                    self.wfile.write(f.read())
                return

            if path == '/api/menu':
                self.send_json(query_all_menu())
                return

            # serve static files
            filepath = path.lstrip('/')
            if os.path.isfile(filepath):
                self.send_response(200)
                ct = 'text/css' if filepath.endswith('.css') else 'text/html; charset=utf-8'
                self.send_header('Content-Type', ct)
                self.end_headers()
                with open(filepath, 'rb') as f:
                    self.wfile.write(f.read())
            else:
                self.send_json({"error": "not found"}, 404)
        except Exception as e:
            print(f"[iqwqi] ERROR in GET {path}: {e}")
            self.send_json({"error": str(e)}, 500)

    def do_POST(self):
        parsed = urllib.parse.urlparse(self.path)
        if parsed.path == '/api/menu':
            length = int(self.headers.get('Content-Length', 0))
            body = json.loads(self.rfile.read(length)) if length > 0 else {}
            ok, msg = add_food(
                body.get('categoryId', 0),
                body.get('id', ''),
                body.get('name', ''),
                body.get('price', 0)
            )
            if ok:
                self.send_json({"success": True, "message": msg})
            else:
                self.send_json({"success": False, "message": msg}, 400)
        else:
            self.send_json({"error": "not found"}, 404)

    def do_PUT(self):
        parsed = urllib.parse.urlparse(self.path)
        if parsed.path.startswith('/api/menu/'):
            food_id = parsed.path.split('/')[-1]
            length = int(self.headers.get('Content-Length', 0))
            body = json.loads(self.rfile.read(length)) if length > 0 else {}
            ok, msg = update_food(
                body.get('categoryId', 0),
                food_id,
                body.get('name', ''),
                body.get('price', 0)
            )
            if ok:
                self.send_json({"success": True, "message": msg})
            else:
                self.send_json({"success": False, "message": msg}, 400)
        else:
            self.send_json({"error": "not found"}, 404)

    def do_DELETE(self):
        parsed = urllib.parse.urlparse(self.path)
        if parsed.path.startswith('/api/menu/'):
            food_id = parsed.path.split('/')[-1]
            ok, msg = delete_food(food_id)
            if ok:
                self.send_json({"success": True, "message": msg})
            else:
                self.send_json({"success": False, "message": msg}, 400)
        else:
            self.send_json({"error": "not found"}, 404)


if __name__ == '__main__':
    addr = ('', 8000)
    print(f"[iqwqi] KFC POS API server running on http://localhost:8000")
    httpd = http.server.HTTPServer(addr, APIHandler)
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\n[iqwqi] Server stopped.")
        httpd.server_close()
