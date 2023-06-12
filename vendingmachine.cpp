#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <vector>
#include <clocale>
#include <cmath>
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

using namespace std;

wstring admin_password = L"admin";

enum CoinCode {
    FIVE_CENTS,
    TEN_CENTS,
    TWENTY_FIVE_CENTS,
    FIFTY_CENTS,
    ONE_REAL
};
struct Coin {
    wstring repr;
    double value;
    int quantity;
};
struct Payment {
    Coin coins[5] = {
        Coin{L"5  Centavos", 0.05, 0},
        Coin{L"10 Centavos", 0.10, 0},
        Coin{L"25 Centavos", 0.25, 0},
        Coin{L"50 Centavos", 0.50, 0},
        Coin{L"1  Real", 1.00, 0}
    };
    double value = 0.00;
};

struct Product {
    wstring code;
    wstring name;
    double price;
    int quantity;
};
std::vector<Product> products;

double sold_value = 0.00;


// common functions
void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void wait_input() {
    cout << "Pressione qualquer tecla para continuar..." << endl;
#ifdef _WIN32
    _getch();
#else
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    int ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
}

wstring format_code(int code) {
    code += 1;
    return code < 10 ? L"0" + to_wstring(code) : to_wstring(code);
}

wstring get_formatted_price(double price) {
    std::wstringstream stream;
    stream << L"R$ " << std::fixed << std::setprecision(2) << price;
    return stream.str();
}

// uses std::optional to return a product reference or not
Product* get_product_by_code(wstring code) {
    for(auto& product : products) {
        if(product.code == code) {
            return &product;
        }
    }
    return nullptr;
}

void print_product(const Product &product, bool show_price = true, bool show_quantity = false) {
    wcout << product.code << ". ";
    wcout << left << setw(20) << product.name;
    if(show_price)
        wcout << L" - " << get_formatted_price(product.price);
    if(show_quantity)
        wcout << L" - " << product.quantity;
    wcout << endl;
}

void list_products(bool show_price = true, bool show_quantity = false) {
    for(auto& product : products) {
        print_product(product, show_price, show_quantity);
    }
}

void initialize_products() {
    products.emplace_back(Product{L"01", L"Refrigerante Cola", 4.50, 5});
    products.emplace_back(Product{L"02", L"Refrigerante Laranja", 4.50, 5});
    products.emplace_back(Product{L"03", L"Refrigerante Guaraná", 4.50, 5});
    products.emplace_back(Product{L"04", L"Sprite", 4.50, 5});
    products.emplace_back(Product{L"05", L"Água", 3.00, 5});
    products.emplace_back(Product{L"06", L"Suco", 3.50, 5});
    products.emplace_back(Product{L"07", L"Bombom", 2.50, 5});
    products.emplace_back(Product{L"08", L"Bolacha", 2.00, 5});
    products.emplace_back(Product{L"09", L"Salgadinho", 3.00, 5});
    products.emplace_back(Product{L"10", L"Bala", 1.00, 5});
    products.emplace_back(Product{L"11", L"Pipoca", 2.00, 5});
    products.emplace_back(Product{L"12", L"Amendoim", 2.00, 5});
    products.emplace_back(Product{L"13", L"Paçoca", 1.00, 5});
    products.emplace_back(Product{L"14", L"Bolo", 3.00, 5});
}


// admin functions
void add_product() {
    clear_screen();
    wcout << "-> [ADMIN MODE] Adicionar produto <-" << endl;

    Product product;
    wcout << L"Digite o nome do produto: ";
    wcin >> product.name;
    wcout << L"Digite o preço do produto: ";
    wcin >> product.price;
    wcout << L"Digite a quantidade do produto: ";
    wcin >> product.quantity;

    product.code = format_code(products.size());
    products.emplace_back(product);

    wcout << L"Produto adicionado com sucesso!" << endl;
    wait_input();
}

void refill_product() {
    clear_screen();
    wcout << "-> [ADMIN MODE] Repor produto <-" << endl;

    list_products(false, true);
    wcout << L"Digite o código do produto: ";
    wstring code;
    wcin >> code;

    Product* product = get_product_by_code(code);
    if(product == nullptr) {
        wcout << L"Código inválido!" << endl;
        wait_input();
        return;
    }

    wcout << L"Digite a quantidade a ser reposta: ";
    int quantity;
    wcin >> quantity;

    product->quantity += quantity;

    wcout << L"Produto reposto com sucesso!" << endl;
    wait_input();
}

void change_product_price() {
    clear_screen();
    wcout << L"-> [ADMIN MODE] Alterar preço <-" << endl;

    list_products();
    wcout << L"Digite o código do produto: ";
    wstring code;
    wcin >> code;
    Product* product = get_product_by_code(code);
    if(product == nullptr) {
        cout << "Código inválido!" << endl;
        wait_input();
        return;
    }

    wcout << L"Produto selecionado: ";
    print_product(*product);

    wcout << L"Digite o novo preço: ";
    double price;
    wcin >> price;

    product->price = price;

    wcout << L"Preço alterado com sucesso!" << endl;
    wait_input();
}

double get_value_to_profit() {
    double value = 0.00;
    for(auto& product : products) {
        if (product.quantity <= 0)
            continue;
        value += product.price * product.quantity;
    }
    return value;
}

void admin_interface() {
    clear_screen();
    cout << "Selecione uma opção:" << endl;
    cout << "1 - Adicionar produto" << endl;
    cout << "2 - Repor produto" << endl;
    cout << "3 - Alterar preço" << endl;
    cout << "4 - Listar produtos" << endl;
    cout << "5 - Valor total faturado" << endl;
    cout << "6 - Valor a fatuar" << endl;
    cout << "0 - Voltar" << endl;

    string input;
    cin >> input;
    if(input == "0") {
        return;
    }

    if(input == "1") {
        add_product();
    }
    else if(input == "2") {
        refill_product();
    }
    else if(input == "3") {
        change_product_price();
    }
    else if(input == "4") {
        list_products(true, true);
        wait_input();
    }
    else if(input == "5") {
        wcout << L"Valor total faturado: " << get_formatted_price(sold_value) << endl;
        wait_input();
    }
    else if(input == "6") {
        wcout << L"Valor a faturar: " << get_formatted_price(get_value_to_profit()) << endl;
        wait_input();
    }
    else {
        cout << "Opção inválida!" << endl;
        wait_input();
    }

    admin_interface();
}

// user functions
void list_coins_code() {
    wcout << L"Códigos das moedas:" << endl;
    Payment p;
    for(int i = 0; i <= ONE_REAL; ++i) {
        wcout << i << L". " << p.coins[i].repr << endl;
    }
}

void print_payment(Payment payment) {
    wcout << L"Valor total: " << get_formatted_price(payment.value) << endl;
    for(auto& coin : payment.coins) {
        if(coin.quantity == 0)
            continue;
        wcout << L" -> " << coin.quantity << L" moedas de " << coin.repr << endl;
    }
}

Payment parser_string_to_payment(string input) {
    Payment payment;
    for(auto c : input) {
        int cc = (c - '0');
        if(cc < FIVE_CENTS || cc > ONE_REAL) {
            continue;
        }
        payment.coins[cc].quantity += 1;
        payment.value += payment.coins[cc].value;
    }
    return payment;
}

Payment get_change(double payment_value, double price) {
    Payment change;
    double change_value = payment_value - price;
    while(change_value > 0.05) {
        int best_coin = -1;
        double best_coin_value = 0.00;
        for(int i = ONE_REAL; i >= 0; --i) {
            if(change.coins[i].value > change_value)
                continue;
            best_coin = i;
            best_coin_value = change.coins[i].value;
            break;
        }
        
        change.coins[best_coin].quantity += 1;
        change.value += change.coins[best_coin].value;

        change_value -= change.coins[best_coin].value;
    }
    if (change_value > 0.00) {
        change.coins[FIVE_CENTS].quantity += 1;
        change.value += change.coins[FIVE_CENTS].value;
    }
    return change;
}

void sell_product(Product &product) {
    clear_screen();

    if(product.quantity <= 0) {
        wcout << L"Produto indisponível!" << endl;
        return;
    }

    wcout << "Produto selecionado: ";
    print_product(product);

    wcout << endl;

    list_coins_code();
    wcout << L"Digite os códigos das moedas para inserir um valor: ";
    string str_payment;
    cin >> str_payment;
    Payment payment = parser_string_to_payment(str_payment);

    wcout << "Pagamento inserido:\n";
    print_payment(payment);

    if(payment.value < product.price) {
        cout << "Valor insuficiente!" << endl;
        wait_input();
        return;
    }
    
    Payment change = get_change(payment.value, product.price);
    if(change.value > 0.00) {
        cout << "Troco: " << endl;
        print_payment(change);
    }

    cout << "Retire seu produto!" << endl;
    product.quantity -= 1;
    sold_value += product.price;

    wait_input();
}

void user_interface() {
    clear_screen();
    wcout << L"Selecione um produto:" << endl;
    list_products();
    wcout << "0. Voltar" << endl;

    wstring input;
    wcin >> input;
    if(input == L"0") {
        return;
    }

    Product* product = get_product_by_code(input);
    if(product == nullptr) {
        cout << "Código inválido!" << endl;
        user_interface();
    }

    sell_product(*product);

    user_interface();
}

void mode_selection() {
    clear_screen();
    cout << "Qual modo deseja entrar?" << endl;
    cout << "1 - Administrador" << endl;
    cout << "2 - Usuário" << endl;
    wstring input;
    wcin >> input;

    if(input == L"1") {
        wcout << "Digite a senha de administrador: ";
        wcin >> input;

        if(input == admin_password) {
            admin_interface();
        } else {
            cout << "Senha incorreta!" << endl;
        }
    }
    else if(input == L"2") {
        user_interface();
    }
    else {
        cout << "Opção inválida!" << endl;
        wait_input();
    }
    mode_selection();
}

int main() {
    setlocale(LC_ALL, "pt_BR.UTF-8");

    wstring input;
    cout << "Bem vindo a Vending Machine!" << endl;

    initialize_products();
    mode_selection();

    return 0;
}
