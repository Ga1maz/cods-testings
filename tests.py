import customtkinter as ctk
import string
import random
from plyer import notification
import pyperclip
import winsound 

class PasswordGeneratorApp(ctk.CTk):
    def __init__(s):
        super().__init__()
        
        s.title("Генератор паролей")
        s.geometry("400x400")
        s.resizable(False, False)
        
        ctk.set_appearance_mode("System")
        ctk.set_default_color_theme("blue")
        
        s.create_widgets()
    
    def create_widgets(s):
        s.title_label = ctk.CTkLabel(s, text="Генератор паролей", font=("Arial", 20, "bold"))
        s.title_label.pack(pady=10)
        
        s.password_var = ctk.StringVar()
        s.password_entry = ctk.CTkEntry(s, textvariable=s.password_var, 
                                         font=("Arial", 14), width=300, justify="center")
        s.password_entry.pack(pady=10)
        
        s.length_frame = ctk.CTkFrame(s)
        s.length_frame.pack(pady=5)
        
        s.length_label = ctk.CTkLabel(s.length_frame, text="Длина пароля:")
        s.length_label.pack(side="left", padx=5)
        
        s.length_slider = ctk.CTkSlider(s.length_frame, from_=4, to=32, number_of_steps=28, width=200)
        s.length_slider.set(12)
        s.length_slider.pack(side="left", padx=5)
        
        s.length_value = ctk.CTkLabel(s.length_frame, text="12")
        s.length_value.pack(side="left", padx=5)
        
        s.options_frame = ctk.CTkFrame(s)
        s.options_frame.pack(pady=10)
        
        s.uppercase_var = ctk.BooleanVar(value=True)
        s.uppercase_check = ctk.CTkCheckBox(s.options_frame, text="Заглавные буквы (A-Z)", 
                                             variable=s.uppercase_var)
        s.uppercase_check.pack(pady=5, anchor="w")
        
        s.lowercase_var = ctk.BooleanVar(value=True)
        s.lowercase_check = ctk.CTkCheckBox(s.options_frame, text="Строчные буквы (a-z)", 
                                             variable=s.lowercase_var)
        s.lowercase_check.pack(pady=5, anchor="w")
        
        s.digits_var = ctk.BooleanVar(value=True)
        s.digits_check = ctk.CTkCheckBox(s.options_frame, text="Цифры (0-9)", 
                                          variable=s.digits_var)
        s.digits_check.pack(pady=5, anchor="w")
        
        s.symbols_var = ctk.BooleanVar(value=True)
        s.symbols_check = ctk.CTkCheckBox(s.options_frame, text="символы (!@#$%^&*)", 
                                          variable=s.symbols_var)
        s.symbols_check.pack(pady=5, anchor="w")
        
        s.button_frame = ctk.CTkFrame(s)
        s.button_frame.pack(pady=10)
        
        s.generate_button = ctk.CTkButton(s.button_frame, text="сгенерировать", 
                                          command=s.generate_password)
        s.generate_button.pack(side="left", padx=5)
        
        s.copy_button = ctk.CTkButton(s.button_frame, text="копировать", 
                                       command=s.copy_password)
        s.copy_button.pack(side="left", padx=5)
        
        s.length_slider.configure(command=s.update_length_value)
    
    def update_length_value(s, value):
        s.length_value.configure(text=str(int(float(value))))
    
    def generate_password(s):
        length = int(s.length_slider.get())
        use_upper = s.uppercase_var.get()
        use_lower = s.lowercase_var.get()
        use_digits = s.digits_var.get()
        use_symbols = s.symbols_var.get()
        
        if not any([use_upper, use_lower, use_digits, use_symbols]):
            s.show_notification("ошибка", "выберите хотя бы один тип символов", is_error=True)
            return
        
        chars = ""
        if use_upper:
            chars += string.ascii_uppercase
        if use_lower:
            chars += string.ascii_lowercase
        if use_digits:
            chars += string.digits
        if use_symbols:
            chars += string.punctuation
        
        password = ''.join(random.choice(chars) for _ in range(length))
        s.password_var.set(password)
        s.show_notification("успех", "пароль успешно сгенерирован")
    
    def copy_password(s):
        password = s.password_var.get()
        if password:
            pyperclip.copy(password)
            s.show_notification("успех", "пароль скопирован в буфер обмена")
        else:
            s.show_notification("ошибка", "сначала сгенерируйте пароль", is_error=True)
    
    def show_notification(s, title, message, is_error=False):
        if is_error:
            winsound.MessageBeep(winsound.MB_ICONHAND)
        
        notification.notify(
            title=title,
            message=message,
            app_name="Генератор паролей",
            timeout=1,
            toast=is_error
        )

if __name__ == "__main__":
    app = PasswordGeneratorApp()
    app.mainloop()
