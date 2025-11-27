#include <iostream>
#include <fstream>
#include <vector>
#include <string>


using namespace std;

#pragma pack(push, 1)
struct rar_header
{
    uint16_t header_crc;
    uint8_t header_type;
    uint16_t header_flags;
    uint16_t header_size;
};

struct rar_file_header
{
    uint32_t pack_size;
    uint32_t unp_size;
    uint8_t host_os;
    uint32_t file_crc;
    uint32_t file_time;
    uint8_t unp_ver;
    uint8_t method;
    uint16_t name_size;
    uint32_t file_attr;
};
#pragma pack(pop)

int main()
{
    setlocale(LC_ALL, "Russian");


    string filename = "Example.rar";

    // Открываем файл
    ifstream file(filename, ios::binary);

    if(!file.is_open())
    {
        cout << "Не могу открыть файл!" << endl;
        return 1;
    }

    // Узнаем размер файла
    file.seekg(0, ios::end);
    int size = file.tellg();
    file.seekg(0, ios::beg);

    // Читаем весь файл в память
    vector<char> data(size);
    file.read(data.data(), size);
    file.close();

    cout << "Размер архива: " << size << " байт" << endl;

    // Пропускаем сигнатуру (первые 7 байт)
    int pos = 7;
    int file_count = 0;

    // Ищем файлы
    while(pos < size)
    {
        // Читаем заголовок
        rar_header* header = (rar_header*)&data[pos];

        // Если нашли заголовок файла
        if(header->header_type == 0x74)
        {
            // Читаем информацию о файле
            rar_file_header* file_head = (rar_file_header*)&data[pos + sizeof(rar_header)];

            // Место где начинается имя файла
            int name_pos = pos + sizeof(rar_header) + sizeof(rar_file_header);

            // Читаем имя файла
            string filename;
            for(int i = 0; i < file_head->name_size; i++)
            {
                filename += data[name_pos + i];
            }

            // Выводим информацию
            cout << endl;
            cout << "Имя: " << filename << endl;
            cout << "Размер: " << file_head->pack_size << " байт" << endl;
            cout << "Распакованный: " << file_head->unp_size << " байт" << endl;

            // Переходим к следующему заголовку
            pos += header->header_size + file_head->pack_size;
        }
        else
        {
            // Пропускаем другие заголовки
            pos += header->header_size;
        }
    }

    return 0;
}
