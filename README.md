Проект представляет собой консольное приложение на C++, которое записывает метаданные и результаты в базу данных MongoDB. Метаданные сохраняются один раз при создании объекта, а результаты могут добавляться несколько раз с поддержкой вложенных ключей. Взаимодействие с MongoDB реализовано с помощью официальной библиотеки mongocxx.  Проект содержит систему тестирования.

Инструкция по сборке:
1)git clone https://github.com/luppapuppa001/test_task.git Your_folder
2)cd ~/Your_folder
3)cmake -B build
4)cmake --build build
5)./build/mongo_main
6)./build/mongo_tests

Для Docker
1)cd ~/путь/до/проекта
2)touch docker-compose.yml
3)nano docker-compose.yml
в файл всзаписать:

	version: "3.8"
	services:
	  mongo:
	    image: mongo:5.0
	    container_name: mongodb
	    restart: always
	    ports:
	      - "27017:27017"
	    volumes:
	      - mongo-data:/data/db
	
	volumes:
	  mongo-data:

4)docker-compose up -d
5)mongosh
6)use test
