use std::env;
use std::fs;

fn main() {
    let args: Vec<String> = env::args().collect();

    let config = Config::new(&args);

    println!("Search for {}", config.query);
    println!("File Path {}", config.file_path);

    let contents = fs::read_to_string(config.file_path)
        .expect("Should have been able to read the file");
    println!("with text : \n{contents}");
        
}

// Config 구조체가 인자에 대한, 소유권을 가져가야함. 그래서 String
struct Config {
    query: String,
    file_path: String,
}

impl Config {
    // 생성자를 args로 받는다
    fn new(args: &[String]) -> Result<Config, &str> {
        if args.len() < 3 {
            return Err("not enough")
        }

        let query = args[1].clone();
        let file_path = args[2].clone();
    
        Ok(Config{query, file_path})
    }
}
