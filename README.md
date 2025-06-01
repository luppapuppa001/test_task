## Описание проекта

Проект представляет собой консольное приложение на C++, которое записывает метаданные и результаты в базу данных MongoDB.  
Метаданные сохраняются один раз при создании объекта, а результаты могут добавляться несколько раз с поддержкой вложенных ключей.  
Взаимодействие с MongoDB реализовано с помощью официальной библиотеки **mongocxx**.  
Проект содержит систему тестирования.

## Инструкция по сборке

```bash
git clone https://github.com/luppapuppa001/test_task.git Your_folder
cd ~/Your_folder
cmake -B build
cmake --build build
./build/mongo_main
./build/mongo_tests
```

## Для Docker
```bash
cd ~/путь/до/проекта
docker-compose up -d
mongosh
use test
```
