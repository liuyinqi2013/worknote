# -*- coding:utf-8 -*-

# 使用selenium发送qq email
# 
# 1、登录地址：http://mail.qq.com
# 
# 2、发送邮件，要求如下
# 
# (1)收件人:786300875@qq.com
# 
# (2)主题：学习selenium自动化
# 
# (3)任意上传一个附件
# 
# (4)正文：学习selenium自动化测试
# 
# (5)发送邮件后，获取页面邮件，判断是否成功发送


from selenium import webdriver
import time
driver = webdriver.Chrome()
#driver = webdriver.Firefox()
driver.get("http://www.mail.qq.com")

#切换到frame里面
driver.switch_to.frame("login_frame")

#操作登录入口
select_name_input = driver.find_element_by_id("u")
select_name_input.send_keys("445209740")
select_password_input = driver.find_element_by_id("p")
select_password_input.send_keys("")
driver.find_element_by_id("login_button").click()

# 点击写信
time.sleep(1)
driver.find_element_by_id("composebtn").click()

driver.switch_to.frame("mainFrame")

time.sleep(3)
# 收件人
select_sendname_input = driver.find_element_by_xpath("//tr/td[2]/div/div[2]/input[@accesskey='t']")
select_sendname_input.send_keys("445209740@qq.com")

# 主题
select_theme_input = driver.find_element_by_xpath("//div/div/input[@id='subject']")
select_theme_input.send_keys(u"selenium自动化")

# 任意上传一个附件
select_summit_button = driver.find_element_by_xpath("//span/span/span/input[@type='file']")
#select_summit_button.send_keys(r"E:\chh_python\chh.xlsx")

# 正文：学习selenium自动化测试
driver.switch_to.frame(driver.find_element_by_tag_name("iframe"))
select_text = driver.find_element_by_xpath("//html/body")
select_text.send_keys(u"selenium自动化测试邮件")

# 发送
driver.switch_to.parent_frame()
driver.find_element_by_link_text("发送").click()
time.sleep(3)

# 获取页面邮件，判断是否成功发送
driver.find_element_by_link_text("查看发送状态").click()
driver.switch_to.frame("mailSendStatus")
if driver.find_element_by_xpath("//tr/td/span[@id='statusbtn']").text == u"投递成功":
    print ("email sends sucessfully")
else:
    print ("failed")

driver.quit()
